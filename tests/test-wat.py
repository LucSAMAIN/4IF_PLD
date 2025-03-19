#!/usr/bin/env python3

import os
import sys
import subprocess
import argparse
import tempfile
import shutil
import re
from pathlib import Path

def run_command(cmd, logfile=None):
    try:
        output = subprocess.check_output(cmd, stderr=subprocess.STDOUT, shell=True)
        ret = 0
    except subprocess.CalledProcessError as e:
        ret = e.returncode
        output = e.output
    
    if logfile:
        with open(logfile, 'w') as f:
            f.write(output.decode(sys.stdout.encoding))
            f.write(f'\nexit status: {ret}')
    
    return ret, output.decode(sys.stdout.encoding)

def compile_with_emcc(input_file, output_dir):
    """Compile le fichier C avec emcc pour générer un WAT et JS"""
    basename = Path(input_file).stem
    js_file = os.path.join(output_dir, f"{basename}_emcc.js")
    wasm_file = os.path.join(output_dir, f"{basename}_emcc.wasm")
    wat_file = os.path.join(output_dir, f"{basename}_emcc.wat")
    wrapper_js = os.path.join(output_dir, f"{basename}_wrapper.js")
    
    # Compiler en WASM avec emcc - avec des options adaptées à Node.js
    compile_cmd = f"emcc {input_file} -o {js_file} -s WASM=1 -s ENVIRONMENT='node' -s EXPORTED_FUNCTIONS='[\"_main\"]' -s EXPORTED_RUNTIME_METHODS='[\"ccall\",\"cwrap\"]' -s EXIT_RUNTIME=1"
    status1, compile_output = run_command(compile_cmd, os.path.join(output_dir, "emcc_compile.log"))
    
    if status1 != 0:
        return False, None, None, None
    
    # Créer un script wrapper qui va exécuter le module et récupérer la valeur de retour
    # Utiliser des chemins absolus pour être sûr
    abs_js_file = os.path.abspath(js_file)
    
    wrapper_content = f"""
// Script wrapper pour exécuter le module WASM et capturer la valeur de retour
const fs = require('fs');
const path = require('path');

// Logs pour le débogage
console.log("Démarrage du wrapper Node.js");
console.log("Chargement du module depuis: {abs_js_file}");

// Fonction pour capturer la sortie console
const log_file = path.join("{os.path.abspath(output_dir)}", "node_output.log");
fs.writeFileSync(log_file, "Démarrage du wrapper Node.js\\n");

// Variables pour signaler la fin
let hasEnded = false;
let exitTimeout = null;

// Charger le module WASM
try {{
    // Rediriger console.log
    const originalConsoleLog = console.log;
    console.log = function() {{
        const args = Array.from(arguments).join(' ');
        fs.appendFileSync(log_file, args + '\\n');
        originalConsoleLog.apply(console, arguments);
    }};
    
    // Importer le module avec le chemin absolu
    const Module = require('{abs_js_file}');
    
    console.log("Module chargé, en attente d'initialisation...");
    
    // Fonction pour terminer proprement
    function finishExecution(returnValue) {{
        if (hasEnded) return;
        hasEnded = true;
        clearTimeout(exitTimeout);
        
        console.log("RETURN_VALUE:", returnValue);
        fs.appendFileSync(log_file, `RETURN_VALUE: ${{returnValue}}\\n`);
        
        // Attendre un peu pour que les logs s'écrivent
        setTimeout(() => process.exit(0), 100);
    }}
    
    // Attendre l'initialisation du module
    if (Module.calledRun) {{
        console.log("Module déjà initialisé");
        // Le module est déjà initialisé, appeler main directement
        const result = Module.ccall('main', 'number', [], []);
        finishExecution(result);
    }} else {{
        // Définir la fonction à appeler quand le module sera prêt
        Module.onRuntimeInitialized = function() {{
            console.log("Module initialisé, appel de main()");
            try {{
                const result = Module.ccall('main', 'number', [], []);
                finishExecution(result);
            }} catch (e) {{
                console.error("Erreur lors de l'appel à main:", e);
                fs.appendFileSync(log_file, `Erreur: ${{e.toString()}}\\n`);
                process.exit(1);
            }}
        }};
    }}
    
    // Mettre un timeout pour éviter de bloquer
    exitTimeout = setTimeout(() => {{
        console.error("Timeout atteint, arrêt forcé");
        fs.appendFileSync(log_file, "Timeout atteint, arrêt forcé\\n");
        process.exit(1);
    }}, 5000);
    
}} catch (e) {{
    console.error("Erreur lors du chargement du module:", e);
    fs.appendFileSync(log_file, `Erreur de chargement: ${{e.toString()}}\\n`);
    process.exit(1);
}}
"""
    
    with open(wrapper_js, 'w') as f:
        f.write(wrapper_content)
    
    # Convertir le WASM en WAT
    wasm2wat_cmd = f"wasm2wat {wasm_file} -o {wat_file}"
    status2, _ = run_command(wasm2wat_cmd, os.path.join(output_dir, "wasm2wat.log"))
    
    return status1 == 0 and status2 == 0, wasm_file, wat_file, wrapper_js

def run_wasm_file(wasm_file, is_emcc=False, function="main", js_file=None):
    """Exécute un fichier WASM avec wasmer ou Node.js"""
    if is_emcc and js_file:
        # Pour les fichiers générés par emcc, utiliser Node.js pour exécuter notre wrapper
        cmd = f"node {js_file}"
        # Log pour le débogage
        print(f"Exécution de Node.js avec: {cmd}")
    else:
        # Pour les fichiers générés par notre compilateur, continuer à utiliser wasmer
        cmd = f"wasmer run {wasm_file} --invoke {function}"
    
    status, output = run_command(cmd)
    
    # Log pour le débogage
    if is_emcc and status != 0:
        print(f"Erreur lors de l'exécution de Node.js (code {status}):")
        print(output)
        
        # Essayer de lire le fichier de log si disponible
        log_file = os.path.join(os.path.dirname(js_file), "node_output.log")
        if os.path.exists(log_file):
            with open(log_file, 'r') as f:
                log_content = f.read()
                print(f"Contenu du fichier log Node.js:")
                print(log_content)
    
    return status == 0, output

def extract_result(output):
    """Extrait la valeur de retour numérique de la sortie"""
    # Chercher d'abord le marqueur RETURN_VALUE explicite
    for line in output.strip().split('\n'):
        if "RETURN_VALUE:" in line:
            return int(line.split("RETURN_VALUE:")[1].strip())
    
    # Chercher d'abord dans les premières lignes pour le résultat
    lines = output.strip().split('\n')
    # Si la première ligne contient uniquement un nombre, c'est probablement notre résultat
    if lines and lines[0].strip().isdigit():
        return int(lines[0].strip())
    
    # Sinon, chercher le premier nombre isolé dans la sortie
    for line in lines:
        for word in line.strip().split():
            # Si le mot est un nombre entier
            if word.isdigit():
                return int(word)
    
    # Si aucun nombre n'est trouvé, lever une exception
    raise ValueError(f"Could not find a numeric result in output: {output}")

def get_test_number(filename):
    """Extrait le numéro du test à partir du nom de fichier"""
    match = re.match(r'(\d+)_', os.path.basename(filename))
    if match:
        return int(match.group(1))
    return float('inf')  # Mettre à la fin si pas de numéro

def test_wat_file(input_file, output_dir, skip_emcc=False):
    basename = Path(input_file).stem
    test_dir = os.path.join(output_dir, basename)
    os.makedirs(test_dir, exist_ok=True)
    
    # Compile to WAT using the homemade compiler
    wat_file = os.path.join(test_dir, "output.wat")
    compile_cmd = f"../compiler/ifcc -w -o {wat_file} {input_file}"
    compile_status, _ = run_command(compile_cmd, os.path.join(test_dir, "compile.log"))
    
    if compile_status != 0:
        print(f"\033[1;31mFAIL\033[0m: Compilation error for {input_file} with ifcc")
        return False
    
    # Validate WAT syntax using wat2wasm and generate wasm file
    wasm_file = os.path.join(test_dir, "output.wasm")
    validate_cmd = f"wat2wasm {wat_file} -o {wasm_file}"
    validate_status, _ = run_command(validate_cmd, os.path.join(test_dir, "validate.log"))
    
    if validate_status != 0:
        print(f"\033[1;31mFAIL\033[0m: Invalid WAT syntax in {wat_file}")
        return False
    
    # Exécuter le WASM généré par ifcc
    ifcc_success, ifcc_output = run_wasm_file(wasm_file, is_emcc=False)
    if not ifcc_success:
        print(f"\033[1;31mFAIL\033[0m: Execution error for {wasm_file}")
        return False
    
    try:
        # Extraire le résultat de ifcc
        ifcc_result = extract_result(ifcc_output)
        print(f"\033[1;32mPASS\033[0m: {input_file} - ifcc returned {ifcc_result}")
        
        # Compiler et comparer avec emcc si demandé
        if not skip_emcc:
            emcc_success, emcc_wasm, emcc_wat, emcc_js = compile_with_emcc(input_file, test_dir)
            
            if not emcc_success:
                print(f"\033[1;33mWARN\033[0m: Failed to compile {input_file} with emcc, skipping comparison")
            else:
                # Exécuter le JS généré par emcc
                emcc_exec_success, emcc_output = run_wasm_file(emcc_wasm, is_emcc=True, js_file=emcc_js)
                if not emcc_exec_success:
                    print(f"\033[1;33mWARN\033[0m: Execution error for {emcc_js}, skipping comparison")
                else:
                    try:
                        # Extraire le résultat de emcc
                        emcc_result = extract_result(emcc_output)
                        
                        # Comparer les résultats
                        if ifcc_result != emcc_result:
                            print(f"\033[1;31mFAIL\033[0m: {input_file} - ifcc returned {ifcc_result}, emcc returned {emcc_result}")
                            return False
                        else:
                            print(f"\033[1;32mPASS\033[0m: {input_file} - Both compilers returned {ifcc_result}")
                    except (ValueError, IndexError) as e:
                        print(f"\033[1;33mWARN\033[0m: Could not parse emcc output: {str(e)}")
                        print(f"emcc output: {emcc_output}")
    except (ValueError, IndexError) as e:
        print(f"\033[1;31mFAIL\033[0m: {input_file} - Could not parse ifcc output: {str(e)}")
        print(f"ifcc output: {ifcc_output}")
        return False
    
    return True

def main():
    parser = argparse.ArgumentParser(description="Test WebAssembly Text format generation and compare with emcc")
    parser.add_argument('input', nargs='+', help='Input .c files or directories')
    parser.add_argument('-o', '--output', default='wat-test-output', help='Output directory')
    parser.add_argument('--skip-emcc', action='store_true', help='Skip comparison with emcc')
    args = parser.parse_args()
    
    # Verify required tools
    for tool in ["wat2wasm", "wasmer"]:
        if shutil.which(tool) is None:
            print(f"Error: {tool} command not found. Please install it.")
            return 1
    
    if not args.skip_emcc:
        for tool in ["emcc", "wasm2wat", "node"]:
            if shutil.which(tool) is None:
                print(f"Warning: {tool} command not found. Comparison with emcc will be skipped.")
                print("Install Emscripten, WebAssembly Binary Toolkit, and Node.js to enable this feature.")
                args.skip_emcc = True
                break
    
    # Collect all .c files
    input_files = []
    for path in args.input:
        if os.path.isfile(path) and path.endswith('.c'):
            input_files.append(path)
        elif os.path.isdir(path):
            for root, _, files in os.walk(path):
                input_files.extend(
                    os.path.join(root, f) for f in files if f.endswith('.c')
                )
    
    if not input_files:
        print("No .c files found")
        return 1
    
    # Trier les fichiers par numéro de test
    input_files.sort(key=get_test_number)
    
    # Create output directory
    os.makedirs(args.output, exist_ok=True)
    
    # Run tests
    success = 0
    total = len(input_files)
    
    for input_file in input_files:
        if test_wat_file(input_file, args.output, skip_emcc=args.skip_emcc):
            success += 1
    
    print(f"\nResults: {success}/{total} tests passed")
    return 0 if success == total else 1

if __name__ == '__main__':
    sys.exit(main())
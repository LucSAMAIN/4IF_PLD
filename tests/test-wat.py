#!/usr/bin/env python3

import os
import sys
import subprocess
import argparse
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
    
    return ret

def test_wat_file(input_file, output_dir):
    basename = Path(input_file).stem
    test_dir = os.path.join(output_dir, basename)
    os.makedirs(test_dir, exist_ok=True)
    
    # Compile to WAT using the homemade compiler
    wat_file = os.path.join(test_dir, "output.wat")
    compile_cmd = f"../compiler/ifcc -w {input_file} > {wat_file}"
    compile_status = run_command(compile_cmd, os.path.join(test_dir, "compile.log"))
    
    if compile_status != 0:
        print(f"\033[1;31mFAIL\033[0m: Compilation error for {input_file}")
        return False
    
    # Validate WAT syntax using wat2wasm and generate wasm file
    wasm_file = os.path.join(test_dir, "output.wasm")
    validate_cmd = f"wat2wasm {wat_file} -o {wasm_file}"
    validate_status = run_command(validate_cmd, os.path.join(test_dir, "validate.log"))
    
    if validate_status != 0:
        print(f"\033[1;31mFAIL\033[0m: Invalid WAT syntax in {wat_file}")
        return False
    
    print(f"\033[1;32mPASS\033[0m: {input_file}")
    return True

def main():
    parser = argparse.ArgumentParser(description="Test WebAssembly Text format generation")
    parser.add_argument('input', nargs='+', help='Input .c files or directories')
    parser.add_argument('-o', '--output', default='wat-test-output', help='Output directory')
    args = parser.parse_args()
    
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
    
    # Create output directory
    os.makedirs(args.output, exist_ok=True)
    
    # Run tests
    success = 0
    total = len(input_files)
    
    for input_file in input_files:
        if test_wat_file(input_file, args.output):
            success += 1
    
    print(f"\nResults: {success}/{total} tests passed")
    return 0 if success == total else 1

if __name__ == '__main__':
    sys.exit(main())
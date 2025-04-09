# Web Assembly

Le Web Assembly a 2 versions : 
- .wat qui est compréhensible pour un humain et qui ressemble assez à de l'assembler
- .wasm qui est très difficile à comprendre 

Cependant on ne peut exécuter que les codes en .wasm mais on peut utiliser WebAssembly Toolkit pour faire les conversions

Pour pouvoir convertir un .wat en .wasm il faut installer wabt
```sudo apt install wabt```
puis on convertir en faisant : ```wat2wasm fichier.wat -o fichier.wasm```

Pour pouvoir exécuter du code wasm sur linux on peut installer wasmer
```curl https://get.wasmer.io -sSfL | sh```

puis exécuter avec wasmer : ```wasmer votre_fichier.wasm --invoke main``` pour exécuter la fonction main

Pour tester si notre compiler en WebAssembly fonctionne correctement, on va le comparer au code compilé par Emscripten qui est le compiler en WebAssembly le plus connu : 

On commence par installer Emscripten : ```sudo apt-get install emscripten```
Puis on peut simplement utilise ```emcc``` tout comme ```gcc```


if else : 
(block $after_if
  (block $else_part
    ;; Test de condition
    (br_if $else_part (i32.eqz (condition)))
    
    ;; Code du "then"
    ...
    (br $after_if)
  )
  
  ;; Code du "else"
  ...
)


while : 

(block $end_label
  (loop $start_label
    ;; Condition
    (br_if $end_label (i32.eqz (condition)))
    
    ;; Corps de la boucle
    ...
    
    ;; Retour au début
    (br $start_label)
  )
)


# Pour les if-else :
- Implémentation de JumpFalse::gen_wat pour générer du code conditionnel en utilisant l'instruction if de - - - WebAssembly
- Intégration complète des blocs de code pour les branches "then" et "else"
- Élimination des sauts explicites vers les blocs de fin qui sont gérés implicitement par WebAssembly
# Pour les while :
- Implémentation d'une détection des structures de boucle dans le code IR
- Transformation des boucles en utilisant la combinaison block et loop de WebAssembly
- Utilisation de br_if pour sortir conditionnellement de la boucle
- Utilisation de br pour retourner au début de la boucle
# Améliorations générales :
- Meilleure organisation du code généré en traitant séparément les blocs normaux, les if-else et les while
- Élimination de la génération en double des blocs qui font partie des structures de contrôle
- Utilisation correcte des étiquettes pour les branchements

Cette approche est équivalente à la logique des sauts en x86, mais adaptée aux contraintes de WebAssembly où - les sauts directs n'existent pas. Les concepts sont similaires :
Un jump conditionnel en x86 (je, jne) devient un br_if en WebAssembly
Un jump inconditionnel en x86 (jmp) devient un br en WebAssembly
Une boucle while utilise les instructions block et loop pour définir les points d'entrée et de sortie

Execption pour la div par 0 (car gcc l'accepte et pas emcc)
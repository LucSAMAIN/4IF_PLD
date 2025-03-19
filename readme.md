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



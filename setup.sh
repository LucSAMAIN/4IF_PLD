#!/bin/bash

# Script d'installation des dépendances pour les tests IFCC (WAT/WASM)
# Ce script installe les outils nécessaires pour exécuter ifcc-wat-test.py et ifcc-test.py

set -e  # Arrêter le script en cas d'erreur

# Fonction pour afficher les messages en couleur
print_rouge() {
    echo -e "\033[1;31m$1\033[0m"
}

print_vert() {
    echo -e "\033[1;32m$1\033[0m"
}

print_jaune() {
    echo -e "\033[1;33m$1\033[0m"
}

print_vert "Installation des dépendances pour les tests IFCC (WAT/WASM)"

# Vérifier si on est dans un environnement WSL/Linux
if [[ "$(uname -a)" != *"Linux"* ]]; then
    print_rouge "Ce script est conçu pour fonctionner sous Linux ou WSL"
    exit 1
fi

# Mise à jour des dépôts
print_jaune "Mise à jour des dépôts..."
sudo apt-get update

# Installation des outils de base nécessaires pour la compilation
print_jaune "Installation des outils de base pour la compilation..."
sudo apt-get install -y build-essential cmake python3 python3-pip git curl wget

# Installation de Node.js (nécessaire pour exécuter le WASM)
if ! command -v node &> /dev/null; then
    print_jaune "Installation de Node.js..."
    curl -fsSL https://deb.nodesource.com/setup_20.x | sudo -E bash -
    sudo apt-get install -y nodejs
else
    print_vert "Node.js est déjà installé"
fi

# Vérifier la version de Node.js
NODE_VERSION=$(node -v)
print_vert "Version de Node.js installée: $NODE_VERSION"

# Installation de Emscripten (pour emcc)
if ! command -v emcc &> /dev/null; then
    print_jaune "Installation d'Emscripten..."
    
    # Cloner le dépôt emsdk
    git clone https://github.com/emscripten-core/emsdk.git
    
    # Configurer emsdk
    cd emsdk
    ./emsdk install latest
    ./emsdk activate latest
    
    # Ajouter emsdk au PATH de l'utilisateur
    EMSDK_SETUP="source \"$(pwd)/emsdk_env.sh\""
    if ! grep -q "$EMSDK_SETUP" ~/.bashrc; then
        echo "$EMSDK_SETUP" >> ~/.bashrc
        print_vert "Emscripten ajouté à votre PATH dans .bashrc"
    fi
    
    cd ..
    
    # Activer pour la session courante
    source ./emsdk/emsdk_env.sh
else
    print_vert "Emscripten est déjà installé"
fi

# Installation de wabt (WebAssembly Binary Toolkit) pour wat2wasm
if ! command -v wat2wasm &> /dev/null; then
    print_jaune "Installation de wabt (WebAssembly Binary Toolkit)..."
    
    # Installation des dépendances pour la compilation
    sudo apt-get install -y cmake g++ gcc ninja-build
    
    # Cloner le dépôt wabt
    git clone --recursive https://github.com/WebAssembly/wabt.git
    
    # Compiler wabt
    cd wabt
    mkdir build
    cd build
    cmake .. -DBUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release
    cmake --build .
    
    # Installer les outils dans /usr/local/bin
    sudo cp wat2wasm wasm2wat wasm-interp wasm-objdump wasm-strip /usr/local/bin/
    
    cd ../..
    
    print_vert "wabt installé avec succès"
else
    print_vert "wabt est déjà installé"
fi

# Vérifier les installations
print_jaune "Vérification des installations..."

echo -n "node: "
if command -v node &> /dev/null; then
    print_vert "OK"
else
    print_rouge "NON INSTALLÉ"
fi

echo -n "emcc: "
if command -v emcc &> /dev/null; then
    print_vert "OK"
else
    print_rouge "NON INSTALLÉ"
    print_jaune "Si emcc n'est pas détecté, essayez de redémarrer votre terminal ou exécutez 'source ./emsdk/emsdk_env.sh'"
fi

echo -n "wat2wasm: "
if command -v wat2wasm &> /dev/null; then
    print_vert "OK"
else
    print_rouge "NON INSTALLÉ"
fi

print_vert "\nInstallation terminée !"
print_jaune "REMARQUE: Pour que les changements soient pris en compte dans cette session, exécutez:"
echo "source ./emsdk/emsdk_env.sh"
print_jaune "Ou redémarrez votre terminal." 
# IFCC compiler

## Introduction

The IFCC compiler is a compiler for a subset of the C programming language. It handles x86_64 and WebAssembly (WASM) architectures. It uses our own IR (Intermediate Representation) to help handle the compilation process with the different architectures and optimizations.

## Build

To build the IFCC compiler, go to the `compiler` directory and use the following command:

```bash
make
```

This will create the `ifcc` executable in the `compiler` directory.

## Usage

To compile your C program, use the following command:

```bash
path/to/ifcc [-h] [-v] [-o <output_file>] <source_file.c>
```

The options are :
- `-o <output_file>`: Specify the output file name
- `-v`: Enable verbose mode, which prints the symbol table in the output
- `-h`: Show help message

## WebAssembly

Si vous souhaitez compiler en WebAssembly, il faut installer différents programmes dont Node.js. 

Vous pouvez lancer setup.sh qui le fera à votre place, si vous avez les droits nécessaires évidemment.
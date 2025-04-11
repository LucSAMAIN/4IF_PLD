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
path/to/ifcc [-h] [-v] [-o <output_file>] [-w] <source_file.c>
```

The options are :
- `-o <output_file>`: Specify the output file name
- `-v`: Enable verbose mode, which prints the symbol table in the output
- `-h`: Show help message
- `-w`: Generate WebAssembly code instead of x86_64
- `<source_file.c>`: The input C source file to be compiled

## WebAssembly

If you want to compile to WebAssembly, you need to install different programs including Node.js. 

You can run setup.sh which will do it for you, if you have the necessary rights of course.
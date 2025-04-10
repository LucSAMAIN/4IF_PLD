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
path/to/ifcc <source_file.c>
```
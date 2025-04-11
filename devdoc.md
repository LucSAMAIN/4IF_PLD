# Execution
The roles of the visitors are described in the next sections

1. Parse the arguments
2. Tokenize and parse the input program
3. ContinueBreakVisitor
4. SymbolTableVisitor
5. TypeCheckVisitor
6. IRGenVisitor
7. Generate the backend code

## Parse the arguments
We have a simple command line argument parser. It takes the following arguments:
- `-o <output_file>`: Specify the output file name
- `-v`: Enable verbose mode, which print the symbol table in the output
- `-h`: Show help message
- `-w`: Generate WebAssembly code instead of x86_64
- `<source_file.c>`: The input C source file to be compiled

## Tokenize and parse the input program
The input program is tokenized and parsed using ANTLR4. The parser generates a parse tree according to our grammar written in `ifcc.g4`, which is then traversed by the visitors to perform various tasks.

## ContinueBreakVisitor
The `ContinueBreakVisitor` is responsible for checking the usage of `continue` and `break` statements in loops. It ensures that these statements are only used inside loops. If a `continue` or `break` statement is found outside of its valid context, an error is reported.

## SymbolTableVisitor
The `SymbolTableVisitor` is responsible for creating the symbol table. It traverses the parse tree and collects information about variables, functions, and their types. It computes the adresses of the variables and also handles the scoping rules, ensuring that variables are declared and used in the correct scope. Finally, it checks that the function calls are valid and that the arguments passed to the functions match their definitions.

## TypeCheckVisitor
The `TypeCheckVisitor` is responsible for checking the types of expressions and statements in the program. It ensures that the types are compatible and that operations are performed on the correct types. It also checks for type errors, such as assigning a value of one type to a variable of another incompatible type. It writes errors if the types are incompatible and warnings if an implicit conversion is done.

## IRGenVisitor
The `IRGenVisitor` is responsible for generating the intermediate representation (IR) of the program. It traverses the parse tree and generates IR instructions for each statement and expression. The IR is a representation of the program that is easier to optimize and translate to machine code. The IR is generated in a form that is independent of the target architecture, allowing for easier optimization and code generation. Our IR uses CFG (Control Flow Graph) to represent the flow of control in the program.

## Generate the backend code
The backend code is generated using the CFGs created by the `IRGenVisitor`. The backend code is generated for the target architecture (x86_64 or WebAssembly) and is optimized for performance. The backend code generation is done using a set of rules that translate the IR instructions into the target architecture's assembly language.

# Summary

![image](schema_execution.png)
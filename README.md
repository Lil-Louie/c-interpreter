# C Interpreter

A lightweight interpreter for a subset of the C programming language, written in modern C++. This project was developed from scratch to demonstrate a deep understanding of compiler design, tokenization, parsing, abstract syntax tree (AST) and concrete syntax tree (CST) construction, as well as function evaluation and scope management.

## Overview

This interpreter simulates how a C-like language is parsed and executed. It processes `.c` files through several stages: lexical analysis, recursive descent parsing, syntax tree generation, and evaluation. It supports function declarations, conditionals, loops, return statements, and arithmetic expressions with operator precedence.

## Features

- Tokenizer (Lexer) to classify and segment the input stream
- Recursive descent parser to generate a Concrete Syntax Tree (CST)
- LCRS (Left-Child, Right-Sibling) representation for CST
- Abstract Syntax Tree (AST) construction for evaluation
- Symbol Table to handle scoping, types, and identifiers
- Supports:
  - Function declarations and calls
  - Arithmetic expressions with precedence
  - Control structures: `if`, `else`, `while`, `for`
  - Return value handling via a postfix evaluation vector
- Modular design with clean class separation

## Technologies

- C++17
- CMake
- Object-Oriented Programming
- Compiler Design Principles

## Project Structure

```plaintext
c-interpreter/
├── CS460_A6_Interpreter      # Compiled binary (CMake build output)
├── INTERP                    # Executable from manual g++ build
├── Makefile                  # Manual build configuration
├── CMakeLists.txt            # CMake build configuration
├── correct outputs for test files/
│   └── ...                   # Output logs for correctness verification
├── programming_assignment_6-test_file_X.c
│   └── ...                   # Test input files written in C
├── Token.cpp/.hpp/.o        # Tokenizer logic and types
├── Tokenizer.cpp/.hpp/.o    # Lexer that generates tokens from input
├── Parser.cpp/.hpp/.o       # Builds syntax trees (AST/CST)
├── CST.cpp/.hpp/.o          # Concrete Syntax Tree logic
├── CSTNode.hpp              # Nodes for CST
├── SymbolTable.cpp/.hpp/.o  # Variable/function scope & resolution
├── interpreter.cpp/.h/.o    # Evaluator logic and interpreter entry
├── main.cpp/.o              # Entry point (reads & executes .c file)
├── README.md                # Project documentation (this file)
```

How to Compile & Run

Option 1: Compile Manually Using g++
```bash
g++ main.cpp Token.cpp Tokenizer.cpp Parser.cpp CST.cpp SymbolTable.cpp -o INTERP
./INTERP programming_assignment_6-test_file_1.c
```
This builds and runs the interpreter directly from source using g++.

```bash
Option 2: Using the Makefile
make
./INTERP programming_assignment_6-test_file_1.c
```
```bash
Option 3: Using CMake (Preferred)
mkdir build
cd build
cmake ..
make
./CS460_A6_Interpreter ../programming_assignment_6-test_file_1.c

```


Example Input

```
int main() {
    return sum_of_first_n_squares(5);
}

int sum_of_first_n_squares(int n) {
    int sum = 0;
    for (int i = 1; i <= n; i = i + 1) {
        sum = sum + i * i;
    }
    return sum;
}
```

Purpose

This project was created as part of an advanced Computer Science course to demonstrate:

Manual parser construction (recursive descent)
Syntax tree design and evaluation
Intermediate-level C++ development with clean architecture
A deep understanding of how interpreters and compilers function
Academic Context

Developed as part of CS 460: Interpreter Design at Sonoma State University (Fall 2024).

- `.gitignore` or license
- How return values are handled with the postfix vector strategy

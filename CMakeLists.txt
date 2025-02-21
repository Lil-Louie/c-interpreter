cmake_minimum_required(VERSION 3.26)
project(CS460_A6_Interpreter)

set(CMAKE_CXX_STANDARD 11)

add_executable(CS460_A6_Interpreter main.cpp Parser.cpp Token.cpp Tokenizer.cpp CST.cpp
        SymbolTable.cpp
        interpreter.cpp
        interpreter.cpp)

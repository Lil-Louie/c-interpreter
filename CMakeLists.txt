cmake_minimum_required(VERSION 3.26)
project(CS460_A3_RDP_CLion_)

set(CMAKE_CXX_STANDARD 11)

add_executable(CS460_A3_RDP main.cpp Parser.cpp Token.cpp Tokenizer.cpp CST.cpp
        SymbolTable.cpp)

#include <iostream>
#include <fstream>
#include <string>
#include "Token.hpp"
#include "CST.hpp"
#include "CSTNode.hpp"
#include "SymbolTable.h"
#include <vector>
#include <stack>

#ifndef CS460_A6_INTERPRETER_INTERPRETER_H
#define CS460_A6_INTERPRETER_INTERPRETER_H


class interpreter {
public:
    interpreter(CST* AST, SymbolTableList symbol_table_list) : AST(AST), symbol_table_list(symbol_table_list) {}
    void assignAddress();
    void evaluateExpression(CSTNode *root, Token token);
    void interpret();

private:
    CST* AST;
    SymbolTableList symbol_table_list;

    //Maybe change datatype
    std::stack<int> stack;

    void assignAddressHelper(CSTNode *root, int address);


};

#endif //CS460_A6_INTERPRETER_INTERPRETER_H

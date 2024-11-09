/** ***************************************************************************
 * @remark CS 460: Definition of Parser class
 *
 * @author Luis Galvez
 *         Xander
 *         Christian Gonzalez
 *         Anthony Manese
 *
 * @file  Parser.hpp
 * @date  Fall 2024
 **************************************************************************** */

#ifndef PROJECT1_PHASE2_PARSER_HPP
#define PROJECT1_PHASE2_PARSER_HPP

#include <iostream>
#include <fstream>
#include <string>
#include "Token.hpp"
#include "CST.hpp"
#include "CSTNode.hpp"
#include "SymbolTable.h"
#include <vector>

class Parser {
public:
    //default constructor, on declaration we need a vector of tokens, then we definie the token as blank, and create a new cst object.
    Parser(std::vector<Token>& incommingVector) : tokenVector(incommingVector) ,cst(new CST()), newStatement(false){};

    //a
    void expect(const std::string& expected_value);

    //functions to check BNF standards
    void program();
    void main_procedure();
    void function_declaration();
    void procedure_declaration();
    void parameter_list();
    void block_statement();
    void compound_statement();
    void statement();
    void return_statement();
    void declaration_statement();
    void user_defined_function();
    void getchar_function();
    void printf_statement();
    void assignment_statement();
    void iteration_statement();
    void selection_statement();
    void expression();
    void initialization_statement();
    void boolean_expression();
    void numerical_expression();
    bool relational_expression();
    bool boolean_operator();
    bool numerical_operator();
    bool numerical_operand();
    bool datatype_specifier();
    bool reserved_word();
    void identifier_and_identifier_array_list();
    void identifier_array_list();
    void identifier_list();
    void identifier();
    void single_quoted_string();
    void double_quoted_string();
    void printTree() { cst->printTree(); }
    void PrintSymbolTableLL();
    void convertToAST();

    //function to get get our cst tree with out tokens.
    CST* parse();
private:

    //possition in our vector which is passed in, and current scope
    int index = 0;
    int  scope = 0;
    bool newStatement = false;
    bool inForLoop = false;
    bool inDeclaration = false;
    bool inParamList = false;
    bool inFunction = false;
    bool inImportantExp = false;
    //vector passed in on declaration
    std::vector<Token> tokenVector;
    //concrete syntax tree object
    CST* cst;
    //symbol table class where we store our symbol table
    SymbolTableList symbol_table_list;
    //new symbol table element which we fill with data, then clear each time we grab a symbol.
    SymbolTable new_symbol_table;

    //need a vector that contain vector of symbol objects.
    //reason, each vector in our vectors object represents the param list for each functions parameters
    //with the symboltable objects in those representing the params of that function.
    std::vector<std::vector<SymbolTable>> paramLists;
    //temp vector to store the params of each function that will be added to the paramlists.
    std::vector<SymbolTable> tempParamList;
    //list of names used already and their scope
    std::vector<std::pair<std::string, int>> takenNames;
    std::string tempFunctionName = "";
};


#endif //PROJECT1_PHASE2_PARSER_HPP


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
#include <map>


class Parser {
public:
    //default constructor, on declaration we need a vector of tokens, then we definie the token as blank, and create a new cst object.
    Parser(std::vector<Token>& incommingVector) : tokenVector(incommingVector) ,cst(new CST()), newStatement(false){};

    //function wihch expects the given token, if its not the expected token then throw error, otherwise add token to cst
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

    //function to get get our cst tree with out tokens.
    CST* parse();

    //converts our cst to ast
    CST* convertToAST();

    //New to intepreter,
    //assigns a address in each cstnode
    void assignAddress();

    //look up function that checks our list of stored functions and there memory addresses.
    int lookUpFunction(std::string functionName);

    //look up function that checks our list of stored functions and there memory addresses.
    std::string lookUpFunctionFromAddress(int addressloc);

    //adds a feunction name and memory location
    void addFunction(std::string functionName, int addressLoc);

    //function to evaluate a post fix expression of token in a vector
    void postFixEval(std::vector<Token> postfix,int callStartAddress);

    //function to evaluate a post fix expression of token in a vector
    bool postFixEvalBool(std::vector<Token> postfix,int callStartAddress);

    //Function to evaluate a post fix expression (for array of chars)
    void postFixEvalChar(std::vector<Token> postfix,int callStartAddress);

    //interprets the code
    void interpret();
    

    void evaluateExpression(CSTNode *root, Token token);

    //function that dont seem to wrok and arnt nessesary if our parser is our interpreter.
    //CST* getRootOfAST(){ return cst; }
    //SymbolTableList getRootofSymbolTableList() { return symbol_table_list; }
private:

    //position in our vector which is passed in, and current scope
    int highestAddress;
    int index = 0;
    int  scope = 0;
    bool newStatement = false;
    bool inForLoop = false;
    bool inDeclaration = false;
    bool inParamList = false;
    bool inFunction = false;
    bool inImportantExp = false;
    //used as bool; if inAssignment >= 0 we are in ASSIGNMENT
    int inAssignment = 0;
    bool _else = false;
    int keepF3Address;

    //vector passed in on declaration
    std::vector<Token> tokenVector;

    //Token to hold return value
    Token tempToken = Token("");

    //Track the value of for expression 3
    int forThreeVal = 0;

    //Track whether we are in for expression 3 so that we update forThreeVal instead of the symbolNode
    bool inExp3 = false;

    //Postfix vector of tokens
    //std::vector<Token> postFix;

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

    //interpreter variables and private functions.
    void assignAddressHelper(CSTNode *root, int address);

    //keeps track of the current address of the interpreter
    //int programCounter;
    //std::stack<int> stack;

    //vector which holds tuples. the string represents the function name, and the int represents its memory location.
    std::vector<std::tuple<std::string,int>> functionAddresses;

    //instead of using a stack we want to use a stack vector. notes
    //this vector contains memory addresses. when we start the first memory location is main
    //when we traverse we want to update the variable inside this vector at that starting position as we move down with
    //each call
    //if we run into a function call, then we need to push_back the memory address of that function to this vector
    //then recursively call interp afteward to interp that functino call.
    //once that functioneither returns , or if its a procedure, falls out of scope, then we need to pop the value on the end.
    std::vector<int> callStack;

    //value to return between functions.
    int returnValue = -1;

    //need to tell the difference of symbols with the same name that are in different scopes.
    std::string SymbolLocation = "";

    //Keeps track of all arrays built. The string is the name of the array and the vector holds the array elements. The last int is the maxSize
    //since technically we have to limit the size of the vector to the array size. No error message has been implemented to exit the program
    //if we add more than the maxSize for this project. Can be implemented if further progress wants to be made.
    std::map<std::string, std::vector<int>/*, int*/> arrayMapInt;
    std::map<std::string, std::vector<char>/*, int*/> arrayMapChar;

    //
    int forLoopExpress2Start = -1;
    int forLoopExpress3Start = -1;
    int forLoopScope = -1;
    int forLoopCallLoc = -1;
    int forLoopEnd = -1;
    int forExp3 = 0;

};


#endif //PROJECT1_PHASE2_PARSER_HPP

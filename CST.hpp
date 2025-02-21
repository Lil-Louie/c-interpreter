#ifndef CS460_A2_TOKENIZER_CST_HPP
#define CS460_A2_TOKENIZER_CST_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Token.hpp"
#include "CSTNode.hpp"


//Concrete syntax tree, this is the tree object which we want to return
class CST {
public:
    //default constructer which  sets our first cstnode to nullptr
    CST(): root( nullptr ) {};

    //general fucntions for CST, AST and interpreter
    //function to set the root node to a new cstnode
    void setRoot(CSTNode* node) { root = node; }
    void addChild(CSTNode* nodeToken,Token token);
    void addSibling(CSTNode* nodeToken, Token token);
    void printTree();   //Breadth First Traversal
    CSTNode* getRoot() { return root; }
    void RemoveElement();
    bool tokenOfInterest(Token check);
    CSTNode* getNodeAtAddress(int searchAddress);

    //AST conversion function and infix to postfix for easier processing
    std::vector<Token> yardAlgorithm (std::vector<Token>& infix);
    void cstToAst();   //Breadth First Traversal

    //Intepreter functions
    void interpret();
    CSTNode* getMainNode();
    bool inForLoop = false;

private:
    //object made of one cst node which connects to the other nodes apart of this data structure
    CSTNode* root;

};


#endif //CS460_A2_TOKENIZER_CST_HPP
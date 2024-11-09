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
    //function to set the root node to a new cstnode
    void setRoot(CSTNode* node) { root = node; }
    void addChild(CSTNode* nodeToken,Token token);
    void addSibling(CSTNode* nodeToken, Token token);
    void printTree();   //Breadth First Traversal
    void cstToAst();   //Breadth First Traversal
    void RemoveElement();
    CSTNode* getRoot() { return root; }
    std::vector<Token> yardAlgorithm (std::vector<Token>& infix);
    std::vector<Token> yardAlgorithm2 (std::vector<Token>& infix);
    bool tokenOfInterest(Token check);
    bool inForLoop = false;

private:
    //object made of one cst node which connects to the other nodes apart of this data structure
    CSTNode* root;

};


#endif //CS460_A2_TOKENIZER_CST_HPP

#ifndef CS460_A2_TOKENIZER_CSTNODE_HPP
#define CS460_A2_TOKENIZER_CSTNODE_HPP
#include <iostream>
#include <fstream>
#include <string>
#include "Token.hpp"


/** **************************************************************************************
CstNode class


 *****************************************************************************************/
class CSTNode {
public:
    CSTNode(const Token& token ) : token(token), _leftChild(nullptr), _rightSibling(nullptr), child(false) {}
    CSTNode* getLeft() { return _leftChild;}
    CSTNode* getRight() { return _rightSibling;}
    void leftChild( CSTNode *newLeftChild ) { _leftChild = newLeftChild; }
    void rightSibling(CSTNode *newRightSibling) { _rightSibling = newRightSibling; }
    Token getToken() { return token; }
    void setChild(bool temp)   { child = temp; }        // this is to know when we are in a new statemnt
    // and our tree can refelct when to drop a level

    bool isChild() { return child; }
private:
    Token token;
    bool child = false;
    CSTNode *_leftChild, *_rightSibling;
};



#endif //CS460_A2_TOKENIZER_CSTNODE_H

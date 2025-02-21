#include "Token.hpp"
#include "CST.hpp"
#include "CSTNode.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include<stack>
#include "CSTNode.hpp"
#include "CST.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"
#include <vector>

/** **************************************************************************************
recursively adds a child/
@pre:
@post:
 *****************************************************************************************/
void CST::addChild(CSTNode *root, Token token) {
    if ( root == nullptr )
        return;

    if (root->getLeft() == nullptr && root->getRight() == nullptr) {
        CSTNode* newNode = new CSTNode(token);
        root->leftChild(newNode);
        root->getLeft()->setChild(true);
    }else if (root->getLeft() != nullptr) {
        addChild(root->getLeft(), token) ;
    }else if (root->getRight() != nullptr) {
        addChild(root->getRight(), token);
    }
    return;
}





/** **************************************************************************************
recursively adds a sibling.
@pre:
@post:
 *****************************************************************************************/
void CST::addSibling(CSTNode *root, Token token) {

    if ( root == nullptr )
        return;

    if ( root->getRight() == nullptr && root->getLeft() == nullptr ){
        CSTNode* newNode = new CSTNode( token );
        root->rightSibling( newNode );
    }else if ( root->getRight() != nullptr ){
        addSibling( root->getRight(), token );
    } else if ( root->getLeft() != nullptr ){
        addSibling( root->getLeft(), token );
    }
    return;
}




/** **************************************************************************************
Prints the TREE using BFS
@pre:
@post:
 *****************************************************************************************/
void CST::printTree(){
    if ( root == nullptr ) {
        return;
    }
    std::queue<CSTNode*> q;
    q.push( root );

    while ( !q.empty() ) {

        CSTNode* current = q.front();
        //checks if its a child, if yess then it drops to a new line
        if ( current->isChild() ){
            std::cout <<std::endl;
            std::cout <<" |"<<std::endl;
            std::cout <<"\\|/";
            std::cout << "\n" + current->getToken().getTokenString()<< " --> ";
        } else {
            std::cout << current->getToken().getTokenString() << " --> ";
        }
        q.pop();

        if ( current->getLeft()!= nullptr ){
            q.push( current->getLeft() );
        }
        if ( current->getRight() != nullptr ){
            q.push( current->getRight() );
        }
    }
    std::cout <<std::endl;
}

/** **************************************************************************************
Prints the TREE using BFS
@pre:breadth first traversal of our ast
@post: returns a node that has the same address
 *****************************************************************************************/
CSTNode* CST::getNodeAtAddress(int searchAddress){
    if ( root == nullptr ) {
        std::cout<<"searched for Address: "<<searchAddress<<" but could AST Empty?"<<std::endl;
        throw;
        return root;
    }
    std::queue<CSTNode*> q;
    q.push( root );

    while ( !q.empty() ) {

        CSTNode* current = q.front();

        //if the address matches our search address
        //std::cout<<current->getToken().getAddress()<<std::endl;
        if(current->getLocation() == searchAddress){
            //std::cout<<"found Address: "<<searchAddress<<std::endl;

            //return the node we are at
            return current;
        }

        q.pop();

        if ( current->getLeft()!= nullptr ){
            q.push( current->getLeft() );
        }
        if ( current->getRight() != nullptr ){
            q.push( current->getRight() );
        }
    }

    //fail case
    std::cout<<"searched for Address: "<<searchAddress<<" but could not find it in AST"<<std::endl;
    throw;
    return root;
}

/** **************************************************************************************
creates a abstract syntax tree out of our concrete syntax tree.
@pre:takes our CST
@post:converst CST to a AST
notes:
use yard's algorithm to convert numerical expressions and boolean expressions to postfix
{ = begin block
} = end block
function headers like function int sum_of_first_n_squares (int n) = declaration
int sum; = declaration
number = 3; = assignment ->assignment statements tokens in postfix.
return a; return ->value.
 *****************************************************************************************/
void CST::cstToAst(){


    //if we find nullptr stop
    if ( root == nullptr ) {
        return;
    }

    //our new root
    Token token(0,0);
    token.setIdentifier("BEGIN PROGRAM");

    CSTNode* astRoot = new CSTNode(token);

    //set up a queue
    std::queue<CSTNode*> q;

    //vector to store statement until we reach null ptr for right sibling.
    std::vector<Token> lineByLine;

    //push the root onto the queue
    q.push( root );

    //traverse through our queue
    while ( !q.empty() || !lineByLine.empty()) {

        CSTNode* current = q.front();

        //checks if its a child, if yes then it drops to a new line
        if ( current->isChild() || (current->getLeft() == nullptr && current->getRight() == nullptr) ){
            //in here we want to process the line we just recieved
            //print out our line to see what it is.
            std::cout<<"line: "<<std::endl;
            for(int i = 0;i < lineByLine.size();i++){
                std::cout<<lineByLine[i].getTokenString()<<"->";
            }
            std::cout<<std::endl;

            //if the front of our vector is a identifier with the
            if(lineByLine.front().isIdentifier() && (lineByLine.front().getTokenString() == "function" || lineByLine.front().getTokenString() == "procedure")){

                //insert "DECLARATION" token, and progress to next line.
                std::cout<<"found DECLARATION with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("DECLARATION");

                std::cout<<"lineByLine.front().getTokenString(): "<<lineByLine.front().getTokenString()<<std::endl;

                //if the token is procedure, then set the token to be the main
                if(lineByLine.front().getTokenString() == "procedure" ){
                    //std::cout<<"found main for interpretor -------------------------------------------------------------------------- "<<std::endl;
                    token.setisFunction();

                    if(lineByLine[1].getTokenString() == "main"){
                        token.setIsMain();
                        token.setFunctionName("main");
                        token.print();
                    }else{
                        token.setFunctionName(lineByLine[1].getTokenString());
                        token.print();
                    }

                    //otherwise we have a function so set it as such.
                }else{
                    token.setisFunction();
                    std::cout<<"found function lineByLine[1].getTokenString(): "<<lineByLine[1].getTokenString()<<std::endl;
                    token.setFunctionName(lineByLine[2].getTokenString());
                }

                addChild(astRoot,token);


                lineByLine.clear();
                //if we find a datatype as a identifier
            }else if(lineByLine.front().isIdentifier() && (lineByLine.front().getTokenString() == "int" || lineByLine.front().getTokenString() == "string"|| lineByLine.front().getTokenString() == "char"
                                                           || lineByLine.front().getTokenString() == "bool")){

                //insert "DECLARATION" token, and progress to next line.
                for(int i = 0;i < lineByLine.size();i++){
                    if(lineByLine[i].isIdentifier() && (lineByLine[i].getTokenString() == "int" || lineByLine[i].getTokenString() == "string"||lineByLine[i].getTokenString() == "char" )|| lineByLine[i].isComma() || lineByLine[i].getTokenString() == "bool"){
                        std::cout<<"found DECLARATION with string: "<<lineByLine[i].getTokenString() <<std::endl;
                        Token token(lineByLine[i].getLineNum(), lineByLine[i].getCharPos());
                        token.setIdentifier("DECLARATION");

                        if(lineByLine[i + 1].isArray()){
                            token.setVarName(lineByLine[i + 1].getTokenString());
                            token.setArray();
                        }
                        addChild(astRoot,token);
                    }
                }
                //note if multiuple variable are declared, add a declarations token for each of them before moving to the next line.

                lineByLine.clear();
            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "if" ){

                //insert "IF" token
                std::cout<<"found IF with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("IF");
                addChild(astRoot,token);

                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 1;i < lineByLine.size();i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                lineByLine.clear();
            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "else" ){

                //insert "ELSE" token
                std::cout<<"found ELSE with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("ELSE");
                addChild(astRoot,token);


                lineByLine.clear();
            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "for" ){
                inForLoop = true;
                int keepTrack;
                //loop through to get each foor loop part note, each part goes down as a left child of the previous statement.

                //insert "FOR EXPRESSION 1" token with the postfix operations after it.
                std::cout<<"found FOR EXPRESSION 1 with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token1(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token1.setIdentifier("FOR EXPRESSION 1");
                addChild(astRoot,token1);
                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 2;i < lineByLine.size();i++){
                    keepTrack = i;
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                    if(lineByLine[i].isSemicolon())
                        break;
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                //lineByLine.clear();

                //insert "FOR EXPRESSION 2" token with the postfix operations after it.
                std::cout<<"found FOR EXPRESSION 2 with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token2(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token2.setIdentifier("FOR EXPRESSION 2");
                addChild(astRoot,token2);

                importantTokens.clear();
                //fill important tokens iwth the tokens we care about
                for(int i = keepTrack + 1;i < lineByLine.size();i++){
                    keepTrack = i;
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                    if(lineByLine[i].isSemicolon())
                        break;
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                //insert "FOR EXPRESSION 3" token with the postfix operations after it.
                std::cout<<"found FOR EXPRESSION 3 with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token3(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token3.setIdentifier("FOR EXPRESSION 3");
                addChild(astRoot,token3);

                importantTokens.clear();
                //fill important tokens iwth the tokens we care about
                for(int i = keepTrack + 1;i < lineByLine.size() - 1;i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                inForLoop = true;

                lineByLine.clear();
            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "while" ){

                //insert "WHILE" token
                std::cout<<"found WHILE with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("WHILE");
                addChild(astRoot,token);

                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 1;i < lineByLine.size();i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                lineByLine.clear();
            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "printf" ){

                //insert "PRINTF" token
                std::cout<<"found PRINTF with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("PRINTF");
                addChild(astRoot,token);

                //insert string plus the variables associated with that print f statement
                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 1;i < lineByLine.size();i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " -> ";
                    addSibling(astRoot, postfix[i]);
                }

                lineByLine.clear();

            }else if(lineByLine.front().isIdentifier() && lineByLine.front().getTokenString() == "return" ) {

                //insert "RETURN" token
                std::cout << "found RETURN with string: " << lineByLine.front().getTokenString() << std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("RETURN");
                addChild(astRoot, token);

                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 1;i < lineByLine.size();i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                lineByLine.clear();
                //if we find a brace
            }else if(lineByLine.front().isIdentifier() && lineByLine[1].isLParen() ) {

                //insert "CALL" token
                std::cout << "found CALL with string: " << lineByLine.front().getTokenString() << std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("CALL");
                addChild(astRoot, token);

                //then look through the rest of this line to get the relivant boolean operations
                //temp vector to fill with assignment variables we care about
                std::vector<Token> importantTokens;

                //fill important tokens iwth the tokens we care about
                for(int i = 0;i < lineByLine.size();i++){
                    if(tokenOfInterest(lineByLine[i])){
                        std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                        importantTokens.push_back(lineByLine[i]);
                    }
                }

                //test print out
                for(int i = 0;i < importantTokens.size();i++){
                    std::cout<<importantTokens[i].getTokenString()<<"->";
                }
                //apply yards algorithm on those operations
                std::vector<Token> postfix = yardAlgorithm(importantTokens);

                //insert them into our ast and move to next line.
                for(int i = 0;i < postfix.size();i++) {
                    std::cout << postfix[i].getTokenString() << " ->";
                    addSibling(astRoot, postfix[i]);
                }

                lineByLine.clear();
                //if we find a brace
            }else if(lineByLine.front().isLBrace()){
                //insert "BEGIN BLOCK" token, and progress to next line.
                std::cout<<"found BEGIN BLOCK with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("BEGIN BLOCK");
                addChild(astRoot,token);

                lineByLine.clear();

            }else if(lineByLine.front().isRBrace()){

                //insert "END BLOCK" token, and progress to next line.
                std::cout<<"found END BLOCK with string: "<<lineByLine.front().getTokenString() <<std::endl;
                Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                token.setIdentifier("END BLOCK");

                lineByLine.clear();

                addChild(astRoot,token);
            }else if(lineByLine.front().isIdentifier()){

                //check next element in the vector to see if its a equals
                //check next element in the vector to see if its a equals for an array as well
                if(lineByLine[1].isAssignmentOperator() || lineByLine[4].isAssignmentOperator()){

                    //make start to assignment
                    std::cout<<"found ASSIGNMENT with string: "<<lineByLine.front().getTokenString() <<std::endl;
                    Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
                    token.setIdentifier("ASSIGNMENT");
                    addChild(astRoot,token);

                    //temp vector to fill with assignment variables we care about
                    std::vector<Token> importantTokens;

                    //fill important tokens iwth the tokens we care about
                    for(int i = 0;i < lineByLine.size();i++){
                        if(tokenOfInterest(lineByLine[i])){
                            std::cout<<"adding token to important tokens: "<<lineByLine[i].getTokenString() <<std::endl;
                            importantTokens.push_back(lineByLine[i]);
                        }
                    }

                    //test print out
                    for(int i = 0;i < importantTokens.size();i++){
                        std::cout<<importantTokens[i].getTokenString()<<"->";
                    }

                    //call yard algorithm
                    std::vector<Token> postfix = yardAlgorithm(importantTokens);

                    //test print out and push onto the ast
                    for(int i = 0;i < postfix.size();i++){
                        std::cout<<postfix[i].getTokenString()<<"->";
                        addSibling(astRoot,postfix[i]);
                    }
                }
                lineByLine.clear();
            }

            //then push its new processed tokens onto the AST

            //then clear our vector for the next line in our CST

            //lineByLine.clear();
            //std::cout <<std::endl;
            //std::cout <<" |"<<std::endl;
            //std::cout <<"\\|/";
            //std::cout << "\n" + current->getToken().getTokenString()<< "--> ";
        } else {
            //std::cout << current->getToken().getTokenString() << "--> ";
        }
        q.pop();

        if ( current->getLeft()!= nullptr ){

            lineByLine.push_back(current->getToken());
            q.push( current->getLeft() );
        }
        else if ( current->getRight() != nullptr ){

            // add our elements to our vector
            lineByLine.push_back(current->getToken());

            q.push( current->getRight() );
        } //run the end block code here again if we are at the end of the program we are reading because
            //queue will become empty and won't be able to go through the while loop again
        else if ( current->getToken().isRBrace()){
            //insert "END BLOCK" token, and progress to next line.
            std::cout<<"found END BLOCK with string: "<<lineByLine.front().getTokenString() <<std::endl;
            Token token(lineByLine.front().getLineNum(), lineByLine.front().getCharPos());
            token.setIdentifier("END BLOCK");

            lineByLine.clear();

            addChild(astRoot,token);
        }
    }
    //std::cout <<std::endl;

    root = astRoot;

    std::cout<<"Successfully Created AST ------------------------------------------------------"<<std::endl;

}



/** **************************************************************************************
//function to take a infix expression and conver it to postfix
@pre: takes a vector of tokens in infix
@post: returns a vector with token in infix
 *****************************************************************************************/
std::vector<Token> CST::yardAlgorithm (std::vector<Token>& infix){

    std::vector<Token> postfix;
    std::stack<Token> stack;
    Token assignmentOp(0,0);
    bool isAssignment = false;
    bool finished = false;

    //foreach token in token list
    for(int i = 0; i < infix.size(); i++){
        if ((infix[i].isDoubleQuote() || infix[i].isSingleQuote() || infix[i].isLParen()
             || infix[i].isRParen() || infix[i].isLBracket() || infix[i].isRBracket()) && infix[i].isImportant()){
            postfix.push_back(infix[i]);
        }
        if (infix[i].isAssignmentOperator()){
            isAssignment = true;
            assignmentOp = infix[i];
        }else if ((infix[i].isInt())    || (infix[i].isIdentifier()) || /*(infix[i].isSingleQuote()) || (infix[i].isDoubleQuote()) ||*/
                  (infix[i].isString())/* || (infix[i].isLBracket())   || (infix[i].isRBracket())*/){
            //display token
            std::cout<<"here!"<<std::endl;
            infix[i].print();
            postfix.push_back(infix[i]);

        }else{
            if (infix[i].isLParen())
            {
                //push token on stack
                stack.push(infix[i]);

            }else{

                if (infix[i].isRParen()){
                    finished = false;
                    while (!finished){
                        //top of stack token == LEFT PARENTHESIS
                        if (stack.top().isLParen()){
                            stack.pop();
                            finished = true;
                        }else{
                            //display token at top of stack
                            stack.top().print();
                            postfix.push_back(stack.top());
                            //pop stack
                            stack.pop();
                        }
                    }
                }else{
                    if ((infix[i].isBoolE())   || (infix[i].isBoolNE())  || (infix[i].isBoolLT())  || (infix[i].isBoolGT())   ||
                        (infix[i].isBoolLTE()) || (infix[i].isBoolGTE()) || (infix[i].isBoolAnd()) || (infix[i].isBoolOr())   ||
                        (infix[i].isBoolNot()) || (infix[i].isPlus())    || (infix[i].isMinus())   || (infix[i].isAsterisk()) ||
                        (infix[i].isDivide())  || (infix[i].isModulo())  || (infix[i].isAssignmentOperator())){
                        if (stack.empty()){
                            //push token on stack
                            stack.push(infix[i]);
                        }else{
                            //top of stack token == LEFT PARENTHESIS
                            if (infix[i].isLParen()){
                                stack.push(infix[i]);
                            }else{
                                //token == BOOLEAN_NOT
                                if (infix[i].isBoolNot())
                                {
                                    finished = false;

                                    while (!finished){
                                        if (!stack.empty()){
                                            //top of stack token == BOOLEAN NOT
                                            if (stack.top().isBoolNot()){
                                                //display token at top of stack
                                                stack.top().print();
                                                postfix.push_back(stack.top());
                                                //pop stack
                                                stack.pop();
                                            }else{
                                                //push token on stack
                                                stack.push(infix[i]);
                                                finished = true;
                                            }
                                        }else{
                                            //push token on stack
                                            finished = true;
                                        }
                                    }
                                }else{
                                    if ((infix[i].isAsterisk()) || (infix[i].isDivide()) || (infix[i].isModulo()) || (infix[i].isAssignmentOperator()) ){
                                        finished = false;
                                        while (!finished){
                                            if (!stack.empty()){
                                                if ((stack.top().isBoolNot())  || (stack.top().isAsterisk()) || (stack.top().isDivide()) || (stack.top().isModulo())|| (stack.top().isAssignmentOperator())){
                                                    //display token at top of stack
                                                    stack.top().print();
                                                    postfix.push_back(stack.top());
                                                    stack.pop();
                                                }else{
                                                    //push token on stack
                                                    stack.push(infix[i]);
                                                    finished = true;
                                                }
                                            }else{
                                                //push token on stack
                                                stack.push(infix[i]);
                                                finished = true;
                                            }
                                        }
                                    }else{
                                        if ((infix[i].isPlus()) || (infix[i].isMinus())){
                                            finished = false;
                                            while (!finished){
                                                if (!stack.empty())
                                                {
                                                    if ((stack.top().isBoolNot()) || (stack.top().isAsterisk()) ||
                                                        (stack.top().isDivide())  || (stack.top().isModulo())   ||
                                                        (stack.top().isPlus())    || (stack.top().isMinus())    || (stack.top().isAssignmentOperator()))
                                                    {
                                                        //display token at top of stack
                                                        stack.top().print();
                                                        postfix.push_back(stack.top());
                                                        stack.pop();
                                                    }else{
                                                        //push token on stack
                                                        stack.push(infix[i]);
                                                        finished = true;
                                                    }
                                                }else{
                                                    //push token on stack
                                                    stack.push(infix[i]);
                                                    finished = true;
                                                }
                                            }
                                        }else{
                                            if ((infix[i].isBoolE())   || (infix[i].isBoolNE()) || (infix[i].isBoolLT()) || (infix[i].isBoolGT()) ||
                                                (infix[i].isBoolLTE()) || (infix[i].isBoolGTE()))
                                            {
                                                finished = false;
                                                while (!finished){
                                                    if (!stack.empty())
                                                    {
                                                        if ((stack.top().isBoolNot())            || (stack.top().isAsterisk()) ||
                                                            (stack.top().isDivide())             || (stack.top().isModulo())   ||
                                                            (stack.top().isPlus())               || (stack.top().isMinus())    ||
                                                            (stack.top().isAssignmentOperator()) ||
                                                            (stack.top().isBoolE())              || (stack.top().isBoolNE())   ||
                                                            (stack.top().isBoolLT())             || (stack.top().isBoolGT())   ||
                                                            (stack.top().isBoolLTE())            || (stack.top().isBoolGTE())  ||
                                                            (stack.top().isBoolNE()))
                                                        {
                                                            //display token at top of stack
                                                            stack.top().print();
                                                            postfix.push_back(stack.top());
                                                            stack.pop();
                                                        }else{
                                                            //push token on stack
                                                            stack.push(infix[i]);
                                                            finished = true;
                                                        }
                                                    }else{
                                                        //push token on stack
                                                        stack.push(infix[i]);
                                                        finished = true;
                                                    }
                                                }
                                            }else{
                                                if (infix[i].isBoolAnd()){
                                                    finished = false;
                                                    while (!finished){
                                                        if (!stack.empty()){
                                                            if ((stack.top().isBoolNot())            || (stack.top().isAsterisk()) ||
                                                                (stack.top().isDivide())             || (stack.top().isModulo())   ||
                                                                (stack.top().isPlus())               || (stack.top().isMinus())    ||
                                                                (stack.top().isAssignmentOperator()) ||
                                                                (stack.top().isBoolE())              || (stack.top().isBoolNE())   ||
                                                                (stack.top().isBoolLT())             || (stack.top().isBoolGT())   ||
                                                                (stack.top().isBoolLTE())            || (stack.top().isBoolGTE())  ||
                                                                (stack.top().isBoolNE()))
                                                            {
                                                                //display token at top of stack
                                                                stack.top().print();
                                                                postfix.push_back(stack.top());
                                                                stack.pop();
                                                            }else{
                                                                //push token on stack
                                                                stack.push(infix[i]);
                                                                finished = true;
                                                            }
                                                        }else{
                                                            //push token on stack
                                                            stack.push(infix[i]);
                                                            finished = true;
                                                        }
                                                    }
                                                }else{
                                                    if (infix[i].isBoolOr())
                                                    {
                                                        finished = false;
                                                        while (!finished)
                                                        {
                                                            if (!stack.empty())
                                                            {
                                                                if ((stack.top().isBoolNot())            || (stack.top().isAsterisk()) ||
                                                                    (stack.top().isDivide())             || (stack.top().isModulo())   ||
                                                                    (stack.top().isPlus())               || (stack.top().isMinus())    ||
                                                                    (stack.top().isAssignmentOperator()) ||
                                                                    (stack.top().isBoolE())              || (stack.top().isBoolNE())   ||
                                                                    (stack.top().isBoolLT())             || (stack.top().isBoolGT())   ||
                                                                    (stack.top().isBoolLTE())            || (stack.top().isBoolGTE())  ||
                                                                    (stack.top().isBoolNE()))
                                                                {
                                                                    //display token at top of stack
                                                                    stack.top().print();
                                                                    postfix.push_back(stack.top());
                                                                    stack.pop();
                                                                }else{
                                                                    //push token on stack
                                                                    stack.push(infix[i]);
                                                                    finished = true;
                                                                }
                                                            }else{
                                                                //push token on stack
                                                                stack.push(infix[i]);
                                                                finished = true;
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    while (!stack.empty()){
        //display token at top of stack
        stack.top().print();
        postfix.push_back(stack.top());
        stack.pop();
    }

    if (isAssignment)
        postfix.push_back(assignmentOp);

    return postfix;
}

/** **************************************************************************************
//function
@pre:
@post:
 *****************************************************************************************/
bool CST::tokenOfInterest (Token check){
    if(check.isLParen()     ||      check.isRParen()     ||
       check.isModulo()     ||      check.isPlus()       ||
       check.isMinus()      ||      check.isAsterisk()   ||
       check.isDivide()     ||      check.isInt()        ||
       check.isIdentifier() ||      check.isBoolNE()     ||
       check.isBoolNot()    ||      check.isBoolE()      ||
       check.isBoolGT()     ||      check.isBoolGTE()    ||
       check.isBoolLT()     ||      check.isBoolLTE()    ||
       check.isBoolAnd()    ||      check.isBoolOr()     ||
       check.isBoolFalse()  ||      check.isBoolTrue()   ||
       check.isString()     ||      check.isLBracket()   ||
       check.isRBracket()   ||      check.isSingleQuote()||
       check.isDoubleQuote()||
       check.isAssignmentOperator()
            ){
        return true;
    }
    return false;
}




/* ***********************************************************************
team justice names here:
xander maniaci
Luis Galvez
Christian Gonzalez
Anthony Manese

how to compile program through terminal
1. g++ main.cpp Token.cpp Tokenizer.cpp Parser.cpp CST.cpp SymbolTable.cpp
2. ./INTERP programming_assignment_6-test_file_1.c

how to run makefile
1.  make
2.  ./INTERP programming_assignment_6-test_file_1.c

description:
Write a program in C or C++ that interprets programs written in our
C-like programming language defined in Backus-Naur Form (BNF)
*********************************************************************** */
//important includes
#include <iostream>
#include <fstream>
#include<stack>
#include "CSTNode.hpp"
#include "CST.hpp"
#include "Parser.hpp"
#include "Token.hpp"
#include "Tokenizer.hpp"
#include <vector>

using namespace std;

#include <string>
#include <vector>
#include <unordered_map>


/** **************************************************************************************
function Headers
 *****************************************************************************************/
string commentParser(std::ifstream& inputStream,string fileName);
string parseTokens(Tokenizer& tempTokenizer);
std::vector<Token> buildTokenVector(Tokenizer& sourceFile);

/** **************************************************************************************
driver program
@pre: takes a input c file
@post: interprets the c program
 *****************************************************************************************/
int main(int argc, char *argv[]) {

    //code to read file
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " nameOfAnInputFile" << std::endl;
        exit(1);
    }

    std::ifstream inputStream;
    inputStream.open(argv[1], std::ios::in);    // open for reading

    if(!inputStream.is_open()) {
        std::cout << "Unable top open " << argv[1] << ". Terminating...";
        exit(2);
    }

    inputStream.close();
    //std::cout << "found file: " <<argv[1] << std::endl;
    string filename = argv[1];
    filename.pop_back();
    filename.pop_back();

    //function to remove comments
    string output = commentParser(inputStream,argv[1]);
 //   std::cout<<"Removed comments in program -------------------------------------------------------"<<std::endl;

    string tokenizeFile = filename + " without comments.c";
    ofstream result("./" + tokenizeFile, ios::out);
    result << output;
    result.close();

    Tokenizer tokenizer(tokenizeFile);

    //function to get tokens out of comment free file
    std::vector<Token> tokenVector = buildTokenVector(tokenizer);

//    std::cout<<"Tokenized Program ------------------------------------------------------"<<std::endl;

    //make the parse object, passing it in our token vector.
    Parser CSTparser(tokenVector);

    //call our parse function built into our parser to make the CST
    CST* tree = CSTparser.parse();
    //tree->printTree();

 //   CSTparser.printTree();
 //   std::cout<<"Sucessfully Created CST ------------------------------------------------------"<<std::endl;

//    CSTparser.PrintSymbolTableLL();
    
  //  std::cout<<"Sucessfully Created Symbol Table Linked List ------------------------------------------------------"<<std::endl;

    //call our cst function to convert our cst to ast
    CST* AST = CSTparser.convertToAST();

    //
    // CSTparser.printTree();
 //   std::cout<<"Sucessfully Created Abstract Syntax Tree List ------------------------------------------------------"<<std::endl;

 //   std::cout<<"attempting to create interpreter"<<std::endl;

    //loop through the ast, assigning the tokens in the ast with adresses
    CSTparser.assignAddress();
 //   std::cout<<"Sucessfully Addressed AST ------------------------------------------------------"<<std::endl;
    CSTparser.interpret();

    //interpreter.assignAddress();
    //interpreter.interpret();

    return 0;
}



/** **************************************************************************************
function to build our token vector for the recursive decent parsing
@pre: takes a tokenizer object
@post: returns a vector of tokens
 *****************************************************************************************/
std::vector<Token> buildTokenVector(Tokenizer& sourceFile){
    //cout<<"making token vector. ++++++++++++++++++++++++++++++++++++ <<"<<endl;
    //make a vector to stre tokens
    std::vector<Token> tokenVector;
    //gets the first token
    Token token = sourceFile.getToken();
    //token.print();
    //add first token to vector
    tokenVector.push_back(token);
    //loop through until we reach a eof token
    while (!token.isEOF()) {
        token = sourceFile.getToken();
        token.print();
        tokenVector.push_back(token);
    }
    //cout<<"token vector complete. ++++++++++++++++++++++++++++++++++++ <<"<<endl;
    return tokenVector;
}

/** **************************************************************************************
function to get tokens loop through file getting tokens from the tokenizer until we get to a eof token.
@pre: takes a tokenizer object
@post: makes a list of tokens out of our comment fre output file.
 *****************************************************************************************/
string parseTokens(Tokenizer& tempTokenizer){


 //   cout<< "Token list:"<<endl;

    std::string file = "";
 //   file+= "\nToken list:\n\n";

    //gets the first token of the file
    Token tempToken = tempTokenizer.getToken();

    file += tempToken.print();

    //as long as the token recieved is not a eof keep looping through the file.
    while (!tempToken.isEOF()) {
        //keep grabbing new tokens and setting the next token as temptoken until we
        //run out of tokens.

        tempToken = tempTokenizer.getToken();
        file += tempToken.print();

    }

    return file;



}

/** **************************************************************************************
function to remove comments by using a dfa to traverse through our file.
@pre: takes a ifstream object and a filename
@post: removes the comments in the .c file input and outputs a c file
 *****************************************************************************************/
string commentParser(std::ifstream& inputStream,string fileName){

    inputStream.open(fileName, std::ios::in);

    string file = "";
    int state = 0;
    char c;
    int lineNumber = 1;
    int startOfComment = -1;

    //keep looping through our dfa as long as we have input to read
    while(!inputStream.eof()){

        cout<<c;
        inputStream.get(c);

        //index 0 grabs everything until we reach a /
        if(state == 0 ) {
            //if we find a slash then we want to transition to index 1
            if (c == '/') {
                state = 1;
                //otherwise add the input to the file.
            } else{
                if (c == '"') {
                    state = 5;
                } else if (c == '\n') {
                    ++lineNumber;
                } else if (c == '*') {
                    state = 6;
                }
                file += c;
            }
            // if we find a / then we need to check if the next char is a * or a /
        }else if(state == 1){
            //if we find a slash after the initial slash then we are in a c style comment so progress to index 2
            if(c == '/'){
                //adds two spaces to the file to account for the double slash.
                file +="  ";
                state = 2;
                //otherwise if we find a /* then progress to index 3 c++ style comment
            }else if(c == '*'){
                //adds two spaces to the file to account for the slash star.
                file +="  ";
                state = 3;
                //The beginning of a comment block that we need to keep track of if never closed
                startOfComment = lineNumber;
                // otherwise we are not in a comment back to index 0
            }else{
                //add the / since not a comment
                file +='/';
                //add the new char c as well since not in comment
                file +=c;
                state = 0;
            }
            //if we are in a c style comment then
        }else if(state == 2){
            //if we find a next line char then c style comment is done, back to index 0
            if(c == '\n'){
                file +=c;
                state = 0;
                //keep track of the line we are by increasing every time we encounter a new line
                ++lineNumber;
            }else{
                //replace that comment with spaces
                file +=' ';
            }
            //index for being inside a c++ style comment
        }else if(state == 3){
            //if we find a next line char then c style comment is done, back to index 0
            if(c == '*'){
                file +=' ';
                state = 4;
                //if we find a next line char then preserve it.
            }else if(c == '\n') {
                file += c;
                ++lineNumber;
            }else{
                //replace that comment with spaces
                file +=' ';
            }
            //index to check if c++ style comment should be ending
        }else if(state == 4){
            //if we find a next line char then c style comment is done, back to index 0
            if(c == '/'){
                //set double space to handle places where */ should be
                file +=" ";
                state = 0;
            }else if(c == '*'){
                //set double space to handle places where */ should be
                file +=" ";
            }else if(c == '\n'){
                file +=c;
                ++lineNumber;
            }else{
                //replace that comment with spaces
                file +=" ";
                state = 3;
            }
            //set input in comments to white space unless we find a next line character
        } else if (state == 5){
            if ( c=='"'){
                state = 0;
            }else if (c == '\\'){
                file += c;
                inputStream.get(c);
            }
            file += c;
        } else if (state == 6){
            if (c == '/'){
                startOfComment = lineNumber;
                break;
            }
            file +=c;
            state = 0;
        }
    }
    //output error message if: inside a never ending c++ comment (/* without the */) or if end of c++ comment is read without the starting of the comment (*/ without the /*)
    if ( state == 3 || state == 4 || !inputStream.eof()){
        cout << "ERROR: Program contains C-style, unterminated comment starting on line " << startOfComment << endl;
        exit(1);
    }

    inputStream.close();

    cout<<endl;
 //   cout<<"==================output file=================="<< endl;
    cout<< file;

    return file;



}
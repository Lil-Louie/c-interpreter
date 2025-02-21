#include "interpreter.h"


void interpreter::assignAddress(){
    assignAddressHelper(AST->getRoot(), 0);
}

void interpreter::assignAddressHelper(CSTNode *root, int address) {

    if ( root == nullptr ) {
        return;
    }

    if (root->getRight() != nullptr )
        assignAddressHelper(root->getRight(), address+1);
    else if ( root->getLeft() != nullptr ){
        root->setAddress(address);
        if(root->isFunction()){
            if (root->isMain()){
                symbol_table_list.setAddress( symbol_table_list.lookupSymbol( root->getFunctionName() ), address);
                stack.push(address);
            }
            symbol_table_list.setAddress( symbol_table_list.lookupSymbol( root->getFunctionName() ), address);
        }
        address++;
    }

}


void interpreter::evaluateExpression(CSTNode *root, Token token){

}

void interpreter::interpret() {
    int PC = 0;

    while ( !stack.empty() ){

        CSTNode* mainNode = getMainNode();
        if (mainNode != nullptr) {

            CSTNode *currentNode = mainNode;
            std::string statement = currentNode->getToken().getTokenString();

            if (statement == "DECLARATION") {

            } else if (statement == "DECLARATION" || statement == "BEGIN BLOCK" ||
                       statement == "END BLOCK" || statement == "ELSE") {
                PC++;
            } else if (statement == "ASSIGNMENT") {

            } else if (statement == "IF") {

            } else if (statement == "RETURN") {

            } else if (statement == "PRINTF") {

            } else if (statement == "FOR EXPRESSION 1") {

            } else if (statement == "FOR EXPRESSION 2") {

            } else if (statement == "FOR EXPRESSION 3") {

            }
            std::cout << std::endl;
        }
    }

}


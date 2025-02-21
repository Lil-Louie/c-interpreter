#include "SymbolTable.h"
#include <iostream>
#include <iomanip>

SymbolTableList::SymbolTableList() : head(nullptr) {}

SymbolTableList::~SymbolTableList() {
    SymbolNode* current = head;
    while (current) {
        SymbolNode* temp = current;
        current = current->next_symbol_table_element;
        delete temp;
    }
}

void SymbolTableList::insertSymbol(const SymbolTable& symbol,std::string location) {
    SymbolNode* newNode = new SymbolNode();
    newNode->symbolTable = symbol;
    newNode->next_symbol_table_element = head;
    newNode->SymbolLocation = location;
    head = newNode;
}


void SymbolTableList::printTable(SymbolNode* node) {
    SymbolNode* current = node;
    //if the node is a identifier_name then print like so
    if(current->symbolTable.isParamList == false){
        std::cout << "    IDENTIFIER_NAME: " << current->symbolTable.identifier_name << "\n"
                  << "    IDENTIFIER_TYPE: " << current->symbolTable.identifier_type << "\n"
                  << "           DATATYPE: " << current->symbolTable.datatype << "\n"
                  << "  DATATYPE_IS_ARRAY: " << current->symbolTable.DATATYPE_IS_ARRAY << "\n"
                  << "DATATYPE_ARRAY_SIZE: " << current->symbolTable.datatype_array_size << "\n"
                  << "              SCOPE: " << current->symbolTable.scope << "\n"
                  << "           LOCATION: " << current->SymbolLocation << "\n"
                  << "     VARIABLE VALUE: " << current->variableVal << "\n"
                  << std::endl;
        std::cout << std::endl;
        //otherwise print out the paramlist for a function call. note, first one should print the function its apart of, then the rest should
        //print without the paramlist showing.
    }else{
        if (nameOfParameter != current->symbolTable.identifier_name){
            std::cout << " PARAMETER LIST FOR: " << current->symbolTable.identifier_name << std::endl;
            nameOfParameter = current->symbolTable.identifier_name;
        }
        std::cout << "    IDENTIFIER_NAME: " << current->symbolTable.identifier_type << "\n"
                  << "           DATATYPE: " << current->symbolTable.datatype << "\n"
                  << "  DATATYPE_IS_ARRAY: " << current->symbolTable.DATATYPE_IS_ARRAY << "\n"
                  << "DATATYPE_ARRAY_SIZE: " << current->symbolTable.datatype_array_size << "\n"
                  << "              SCOPE: " << current->symbolTable.scope << "\n"
                  << "           LOCATION: " << current->SymbolLocation << "\n"
                  << "  (PARAM) VAR VALUE: " << current->variableVal << "\n"
                  << std::endl;
        std::cout << std::endl;
    }
}

void SymbolTableList::PrintSymbolTableList() {
    SymbolNode* current = head;
    PrintSymbolTableListHelper(current);
}

void SymbolTableList::PrintSymbolTableListHelper(SymbolNode* head){
    if (head == nullptr){
        return;
    }

    PrintSymbolTableListHelper(head->next_symbol_table_element);
    printTable(head);
}


std::string SymbolTableList::findFunctionDataType(std::string functionToFind) {
    return std::string();
}

std::string SymbolTableList::findFunctionDataTypeHelper(SymbolNode *head, std::string functionToFind) {
    return std::string();
}

//new interp function, to search the look up table and appl
SymbolNode *SymbolTableList::lookupSymbol(const std::string &name) {

    //call our recursive helper
    return lookupSymbolHelper(head/*->next_symbol_table_element*/,name);
}

SymbolNode *SymbolTableList::lookupSymbolHelper(SymbolNode* head,const std::string &name) {
    //if the name matches the the id of the symbolnode then return that node,
    //may need to keep track of if its a param list or not

    //if we hit the end of our symbol table then throw error
    if (head == nullptr){
        std::cout<<"searched for name: "<< name <<" but it is not present in the table."<<std::endl;
        throw;
    }

    //otherwise check if it is the name of the symbol table we are looking for
    if(name == head->symbolTable.identifier_name ){
        //std::cout<<""<<std::endl;
        //printTable(head);
        return head;

        //if ont progress through our linked symbol table list.
    }else{
        return lookupSymbolHelper(head->next_symbol_table_element,name);
    }

}

//new interp function, to search the look up table and appl
SymbolNode *SymbolTableList::lookupSymbolAtLocation(const std::string &name,const std::string &location) {

    //call our recursive helper
    return lookupSymbolAtLocationHelper(head/*->next_symbol_table_element*/,name,location);
}

SymbolNode *SymbolTableList::lookupSymbolAtLocationHelper(SymbolNode* head,const std::string &name,const std::string &location) {
    //if the name matches the the id of the symbolnode then return that node,
    //may need to keep track of if its a param list or not

    //if we hit the end of our symbol table then throw error
    if (head == nullptr){
 //       std::cout<<"searched for name: "<< name <<"in location: "<< location <<" but it is not present in the table."<<std::endl;
   //     std::cout<<"searching for param as a paramlist symbol? "<<std::endl;
        return lookupSymbolParam(location);
        throw;
    }

    //otherwise check if it is the name of the symbol table we are looking for
    if(name == head->symbolTable.identifier_name && head->SymbolLocation == location){
        //std::cout<<""<<std::endl;
        //printTable(head);
        return head;

        //if ont progress through our linked symbol table list.
    }else{
        return lookupSymbolAtLocationHelper(head->next_symbol_table_element,name,location);
    }

}

//new interp function, to search the look up table and appl
SymbolNode *SymbolTableList::lookupSymbolParam(const std::string &functionName) {

    //call our recursive helper
    return lookupSymbolHelper(head/*->next_symbol_table_element*/,functionName);
}

SymbolNode *SymbolTableList::lookupSymbolParamHelper(SymbolNode* head,const std::string &functionName) {
    //if the name matches the the id of the symbolnode then return that node,
    //may need to keep track of if its a param list or not

    //if we hit the end of our symbol table then throw error
    if (head == nullptr){
        std::cout<<"searched for param: "<< functionName <<" but it is not present in the table."<<std::endl;
 //       throw;
    }

    //check if function name is set
    if(functionName == head->symbolTable.identifier_name && head->symbolTable.isParamList == true){
  //      std::cout<<"found param: "<<head->symbolTable.identifier_type<<std::endl;
        //printTable(head);
        return head;

        //if ont progress through our linked symbol table list.
    }else{
        return lookupSymbolHelper(head->next_symbol_table_element,functionName);
    }

}



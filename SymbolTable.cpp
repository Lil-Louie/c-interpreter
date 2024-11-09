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

void SymbolTableList::insertSymbol(const SymbolTable& symbol) {
    SymbolNode* newNode = new SymbolNode();
    newNode->symbolTable = symbol;
    newNode->next_symbol_table_element = head;
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





#ifndef CS460_A3_RDP_CLION__SYMBOLTABLE_H
#define CS460_A3_RDP_CLION__SYMBOLTABLE_H
#include <string>
#include <stack>
#include <vector>
#include <ostream>
#include <iostream>

struct SymbolTable {
    std::string identifier_name;
    std::string identifier_type;
    std::string datatype = "NOT APPLICABLE";
    std::string DATATYPE_IS_ARRAY = "no";
    std::string datatype_array_size = "0";
    int scope = 0;
    bool isParamList = false;
    bool isForVariable;

};

// Define the Node structure for the linked list
struct SymbolNode {
    SymbolTable symbolTable;
    SymbolNode* next_symbol_table_element;
    int _address = 0;
    std::string SymbolLocation = "";
    int variableVal = 0;
};

// Define the SymbolTable class to manage the linked list of symbols
class SymbolTableList {
public:
    SymbolTableList();
    ~SymbolTableList();

    void printTable( SymbolNode* Node);
    void PrintSymbolTableList();
    void insertSymbol(const SymbolTable& symbol,std::string location);
    std::string findFunctionDataType(std::string functionToFind);
    std::string findFunctionDataTypeHelper(SymbolNode* head,std::string functionToFind);
    SymbolNode* lookupSymbol(const std::string& name);
    SymbolNode* lookupSymbolHelper(SymbolNode* head,const std::string& name);
    SymbolNode* lookupSymbolAtLocation(const std::string& name,const std::string &location);
    SymbolNode* lookupSymbolAtLocationHelper(SymbolNode* head,const std::string& name,const std::string &location);
    SymbolNode* lookupSymbolParam(const std::string& functionName);
    SymbolNode* lookupSymbolParamHelper(SymbolNode* head,const std::string& functionName);

    void setVarVal(SymbolNode* node, int value){
        node->variableVal = value;
    }
    int getVarVal(){
        return head->variableVal;
    }
    void setAddress(SymbolNode* current, int address ) {
        current->_address = address;
        //std::cout<<"setting address in symbol node current->_address: "<<current->_address<<std::endl;
    }
    
private:
    SymbolNode* head;
    std::string nameOfParameter = "";

    void PrintSymbolTableListHelper(SymbolNode *head);
};

#endif //CS460_A3_RDP_CLION__SYMBOLTABLE_H
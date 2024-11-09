#ifndef CS460_A3_RDP_CLION__SYMBOLTABLE_H
#define CS460_A3_RDP_CLION__SYMBOLTABLE_H
#include <string>
#include <stack>
#include <vector>

struct SymbolTable {
    std::string identifier_name;
    std::string identifier_type;
    std::string datatype = "NOT APPLICABLE";
    std::string DATATYPE_IS_ARRAY = "no";
    std::string datatype_array_size = "0";
    int scope = 0;
    bool isParamList = false;
   
};

// Define the Node structure for the linked list
struct SymbolNode {
    SymbolTable symbolTable;
    SymbolNode* next_symbol_table_element;
};

// Define the SymbolTable class to manage the linked list of symbols
class SymbolTableList {
public:
    SymbolTableList();
    ~SymbolTableList();

    void printTable( SymbolNode* Node);
    void PrintSymbolTableList();
    void insertSymbol(const SymbolTable& symbol);
    std::string findFunctionDataType(std::string functionToFind);
    std::string findFunctionDataTypeHelper(SymbolNode* head,std::string functionToFind);
    SymbolTableList* lookupSymbol(const std::string& name);
    

private:
    SymbolNode* head;
    std::string nameOfParameter = "";

    void PrintSymbolTableListHelper(SymbolNode *head);
};

#endif //CS460_A3_RDP_CLION__SYMBOLTABLE_H

#include "pycom/semanalyzer/SymbolTable.hpp"


SymbolTable::SymbolTable(std::map <std::string, Symbol> data) {this->data = data;}

void SymbolTable::add(std::string name, Symbol value) {
    this->data.insert_or_assign(name, value);
}

bool SymbolTable::isSymbolExists(std::string name) {
    if(this->data.find(name) == this->data.end()) { 
        return false;
    }
    return true;
}

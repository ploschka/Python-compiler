#include "pycom/semanalyzer/SymbolTableStack.hpp"


void SymbolTableStack::addId(std::string name, Symbol value) {
    this->data.back().add(name, value);
}

void SymbolTableStack::push(SymbolTable table) {
    this->data.push_back(table);
    
}

void SymbolTableStack::pop() {
    this->data.pop_back();
}

bool SymbolTableStack::isSymbolExists(std::string name) {
    for (SymbolTable table : this->data) {
        if (table.isSymbolExists(name)) {
            return true;
        }
    }
    return false;
}

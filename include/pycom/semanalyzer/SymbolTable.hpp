#pragma once

#include <vector>
#include <string>
#include <map>
#include "pycom/semanalyzer/Symbol.hpp"
//#include "pycom/AST/ASTNode.hpp"


class SymbolTable{
private:
    std::map <std::string, Symbol> data;
public:
    SymbolTable(std::map <std::string, Symbol> data);
    void add(std::string name, Symbol value);
    bool isSymbolExists(std::string name);
};
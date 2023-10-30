#pragma once

#include <pycom/semanalyzer/Symbol.hpp>

#include <vector>
#include <string>
#include <map>

class SymbolTable
{
private:
    std::map<std::string, Symbol> data;

public:
    SymbolTable(std::map<std::string, Symbol> data);
    void add(std::string name, Symbol value);
    bool isSymbolExists(std::string name);
};

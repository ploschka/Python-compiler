#include <vector>
#include "pycom/semanalyzer/SymbolTable.hpp"


class SymbolTableStack{
private:
    std::vector<SymbolTable> data;
public:
    SymbolTableStack() = default;
    void addId(std::string name, Symbol value);
    void push(SymbolTable table);
    void pop();
    bool isSymbolExists(std::string name);
};
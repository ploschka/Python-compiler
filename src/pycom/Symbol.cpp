#include "pycom/semanalyzer/Symbol.hpp"

SymbolInteger::SymbolInteger(std::string name, int value)
{
    this->name = name;
    this->value = value;
}
std::string SymbolInteger::getName() { return this->name; }
int SymbolInteger::getValue() { return this->value; }

SymbolFloat::SymbolFloat(std::string name, float value)
{
    this->name = name;
    this->value = value;
}
std::string SymbolFloat::getName() { return this->name; }
float SymbolFloat::getValue() { return this->value; }

SymbolString::SymbolString(std::string name, std::string value)
{
    this->name = name;
    this->value = value;
}
std::string SymbolString::getName() { return this->name; }
std::string SymbolString::getValue() { return this->value; }

SymbolBool::SymbolBool(std::string name, bool value)
{
    this->name = name;
    this->value = value;
}
std::string SymbolBool::getName() { return this->name; }
bool SymbolBool::getValue() { return this->value; }

SymbolFunction::SymbolFunction(std::string name, std::vector<Symbol> params, Symbol returnType)
{
    this->name = name;
    this->params = params;
    this->returnType = returnType;
}
std::string SymbolFunction::getName() { return this->name; }
std::vector<Symbol> SymbolFunction::getParams() { return this->params; }
Symbol SymbolFunction::getReturnType() { return this->returnType; }

#pragma once

#include <string>
#include <vector>


class Symbol {};


class SymbolInteger: Symbol {
private:
    std::string name;
    int value;
public:
    SymbolInteger();
    SymbolInteger(std::string name, int value);
    std::string getName();
    int getValue();
};


class SymbolFloat: Symbol {
private:
    std::string name;
    float value;
public:
    SymbolFloat();
    SymbolFloat(std::string name, float value);
    std::string getName();
    float getValue();
};


class SymbolString: Symbol {
private:
    std::string name;
    std::string value;
public:
    SymbolString();
    SymbolString(std::string name, std::string value);
    std::string getName();
    std::string getValue();
};


class SymbolBool: public Symbol {
private:
    std::string name;
    bool value;
public:
    SymbolBool();
    SymbolBool(std::string name, bool value);
    std::string getName();
    bool getValue();
};


class SymbolFunction: public Symbol {
private:
    std::string name;
    std::vector<Symbol> params;
    Symbol returnType;
public:
    SymbolFunction(std::string name, std::vector<Symbol> params, Symbol returnType);
    std::string getName();
    std::vector<Symbol> getParams();
    Symbol getReturnType();
};
#pragma once
#include <pycom/token/Token.hpp>
#include <set>

struct Symbol
{
    Token token;
    std::set<std::string>::const_iterator type;
    Symbol(Token _token, std::set<std::string>::const_iterator _type);
};

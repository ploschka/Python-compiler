#pragma once
#include "pycom/interface/LexemeInterface.hpp"

class Lexeme: public LexemeInterface
{
private:
    std::string value;
    Type type;
public:
    std::string getValue() const;
    Type getType() const;
    int setValue(std::string value);
    int setType(Type type);
};

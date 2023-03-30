#pragma once
#include <string>

enum class Type {

};

class Lexeme
{
private:
    std::string value;
    Type type;
public:
    std::string getValue() const;
    Type getType() const;
    void setValue(std::string value);
    void setType(Type type);
};

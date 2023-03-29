#pragma once
#include <string>

enum class Type;

class LexemeInterface
{
public:
    virtual std::string getValue() const = 0;
    virtual Type getType() const = 0;
    virtual int setValue(std::string value) = 0;
    virtual int setType(Type type) = 0;
};

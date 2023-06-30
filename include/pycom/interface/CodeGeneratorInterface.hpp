#pragma once

#include <pycom/AST/AST.hpp>

class CodeGeneratorInterface
{
public:
    virtual void generate(AST *_ast) = 0;
    virtual ~CodeGeneratorInterface() = default;
};

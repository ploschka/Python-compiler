#pragma once

#include <pycom/AST/AST.hpp>

class SemanticAnalyzerInterface
{
public:
    virtual ~SemanticAnalyzerInterface() = default;
    virtual void checkSemantics(AST *_tree) = 0;
    virtual void reset() = 0;
};

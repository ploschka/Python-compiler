#pragma once

#include <pycom/AST/AST.hpp>

class SemanticAnalyzerInterface
{
public:
    virtual ~SemanticAnalyzerInterface() = default;
    virtual bool checkSemantics(AST *_tree) = 0;
    virtual void reset() = 0;
};

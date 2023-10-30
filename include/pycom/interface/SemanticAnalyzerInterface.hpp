#pragma once

#include <pycom/AST/AST.hpp>
#include <pycom/AST/ASTNodePublic.hpp>
#include <pycom/semanalyzer/SymbolTableStack.hpp>

class SemanticAnalyzerInterface {
public:
    virtual ~SemanticAnalyzerInterface() = default;
    virtual bool checkSemantics(AST* tree) = 0;
};

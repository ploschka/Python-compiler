#pragma once
#include <pycom/interface/ParserInterface.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <pycom/AST/AST.hpp>
#include <pycom/optimizer/ShortenExpressionVisitor.hpp>
#include <vector>

class Optimizer
{
public:
    void optimize(AST *_ast);
};

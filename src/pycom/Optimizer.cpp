#include <pycom/optimizer/Optimizer.hpp>

void Optimizer::optimize(AST *ast)
{
    ast->accept(new ShortenExpressionVisitor());
}
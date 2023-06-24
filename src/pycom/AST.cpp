#include <pycom/AST/AST.hpp>

AST::AST(BaseASTNode *_root)
{
    this->root = _root;
}

void AST::accept(NodeVisitorInterface *_visitor)
{
    this->root->accept(_visitor);
}

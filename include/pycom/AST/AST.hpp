#pragma once

#include <pycom/AST/ASTNode.hpp>
#include <pycom/interface/NodeVisitorInterface.hpp>

class AST
{
    BaseASTNode *root;

public:
    AST(BaseASTNode *_root);
    void accept(NodeVisitorInterface *_visitor);
};

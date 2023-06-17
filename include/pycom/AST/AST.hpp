#pragma once
#include <pycom/AST/ASTNode.hpp>

class AST {
    BaseASTNode* root;
public:
    AST(BaseASTNode* root);
    void print();
};

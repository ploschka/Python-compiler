#include <pycom/AST/AST.hpp>
#include <pycom/AST/NodePrintVisitor.h>

AST::AST(BaseASTNode *root) {
    this->root = root;
}

void AST::print() {
    this->root->accept(new PrintVisitor());
}

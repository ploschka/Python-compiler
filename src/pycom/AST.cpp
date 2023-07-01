#include <pycom/AST/AST.hpp>
#include <pycom/AST/NodePrintVisitor.h>
#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

AST::AST(BaseASTNode *root) {
    this->root = root;
}

void AST::print() {
    this->root->accept(new PrintVisitor());
}

bool AST::isSemanticallyCorrect(){
    this->root->accept(new SemanticAnalyzer());
    return true;
}

#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

bool SemanticAnalyzer::checkSemantics(AST *_tree)
{
    _tree->accept(visitor.get());
    return visitor->getResult();
}

SemanticAnalyzer::SemanticAnalyzer() : visitor(std::make_unique<SemanticVisitor>()) {}

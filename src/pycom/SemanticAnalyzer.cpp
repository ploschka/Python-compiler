#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

void SemanticAnalyzer::checkSemantics(AST *_tree)
{
    _tree->accept(visitor.get());
}

SemanticAnalyzer::SemanticAnalyzer() : visitor(std::make_unique<SemanticVisitor>())
{
    visitor->stdinit();
}

void SemanticAnalyzer::reset()
{
    visitor->reset();
}

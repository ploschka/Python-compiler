#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

void SemanticAnalyzer::checkSemantics(AST *_tree)
{
    _tree->accept(visitor.get());
}

SemanticAnalyzer::SemanticAnalyzer(block_map_t *_map) : visitor(std::make_unique<SemanticVisitor>(_map))
{
    visitor->stdinit();
}

void SemanticAnalyzer::reset()
{
    visitor->reset();
}

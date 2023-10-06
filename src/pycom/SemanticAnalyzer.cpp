#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

bool SemanticAnalyzer::isSemanticallyCorrect(AST* _tree)
{
    _tree->accept(visitor.get());
    return visitor->getResult();
}

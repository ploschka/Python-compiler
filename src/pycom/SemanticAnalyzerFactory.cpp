#include <pycom/factory/SemanticFactory.hpp>
#include <pycom/semanalyzer/SemanticAnalyzer.hpp>

std::unique_ptr<SemanticAnalyzerInterface> SemanticFactory::create(){
    return std::make_unique<SemanticAnalyzer>();
}

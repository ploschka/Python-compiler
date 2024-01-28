#pragma once

#include <pycom/interface/SemanticAnalyzerInterface.hpp>
#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/utility/BlockTable.hpp>

#include <memory>

class SemanticAnalyzer : public SemanticAnalyzerInterface
{
private:
    std::unique_ptr<SemanticVisitor> visitor;

public:
    SemanticAnalyzer(block_map_t *_map);
    void checkSemantics(AST *_tree) override;
    void reset() override;
};

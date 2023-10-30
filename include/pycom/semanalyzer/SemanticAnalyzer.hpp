#pragma once

#include <pycom/interface/SemanticAnalyzerInterface.hpp>
#include <pycom/semanalyzer/SemanticVisitor.hpp>

#include <memory>

class SemanticAnalyzer : public SemanticAnalyzerInterface
{
private:
    std::unique_ptr<SemanticVisitor> visitor;

public:
    bool checkSemantics(AST *_tree) override;
};

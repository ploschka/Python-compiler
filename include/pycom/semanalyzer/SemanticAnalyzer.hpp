#pragma once

#include <pycom/interface/SemanticAnalyzerInterface.hpp>
#include <pycom/semanalyzer/SemanticVisitor.hpp>
#include <pycom/interface/ErrorEmitterInterface.hpp>

#include <memory>

class SemanticAnalyzer : public SemanticAnalyzerInterface, public ErrorEmitterInterface
{
private:
    std::unique_ptr<SemanticVisitor> visitor;

public:
    SemanticAnalyzer();
    void checkSemantics(AST *_tree) override;
    void reset() override;
    void setEM(ErrorManagerInterface *_em);
};

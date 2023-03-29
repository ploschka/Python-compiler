#pragma once
#include <pycom/interface/SemanticAnalyzerInterface.hpp>
#include <memory>

class SemanticFactory
{
public:
    std::unique_ptr<SemanticAnalyzerInterface> create();
};

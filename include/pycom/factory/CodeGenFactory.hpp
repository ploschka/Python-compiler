#pragma once
#include <pycom/interface/CodeGeneratorInterface.hpp>
#include <memory>

class CodeGenFactory
{
public:
    std::unique_ptr<CodeGeneratorInterface> create();
};

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/CodeGeneratorInterface.hpp>

#include <memory>

class CodeGenFactory
{
public:
    std::unique_ptr<CodeGeneratorInterface> create(llvm::IRBuilder<> *, llvm::Module *, llvm::LLVMContext *);
};

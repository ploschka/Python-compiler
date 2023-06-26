#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/CodeGeneratorInterface.hpp>
#include <pycom/codegen/CodeEmittingNodeVisitor.hpp>

#include <memory>
#include <queue>

class CodeGenerator : public CodeGeneratorInterface
{
private:
    std::unique_ptr<CodeEmittingNodeVisitor> visitor;
    std::queue<llvm::Value*> stored_values;

public:
    void generate(AST *_ast);
    CodeGenerator(llvm::IRBuilder<> *, llvm::Module *, llvm::LLVMContext *);
};

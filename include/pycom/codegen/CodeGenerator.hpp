#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/CodeGeneratorInterface.hpp>
#include <pycom/codegen/CodeEmittingNodeVisitor.hpp>
#include <pycom/codegen/GlobalNodeVisitor.hpp>
#include <pycom/interface/ErrorManagerInterface.hpp>
#include <pycom/interface/ErrorEmitterInterface.hpp>

#include <memory>

class CodeGenerator : public CodeGeneratorInterface, public ErrorEmitterInterface
{
private:
    std::unique_ptr<CodeEmittingNodeVisitor> codevisitor;
    std::unique_ptr<GlobalNodeVisitor> globalvisitor;

public:
    void generate(AST *_ast);
    CodeGenerator(llvm::IRBuilder<> *, llvm::Module *, llvm::LLVMContext *);
    void setEM(ErrorManagerInterface* _em);
};

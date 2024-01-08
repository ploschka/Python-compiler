#pragma once
#include <pycom/lexer/Lexer.hpp>
#include <pycom/parser/Parser.hpp>
#include <pycom/semanalyzer/SemanticAnalyzer.hpp>
#include <pycom/codegen/CodeGenerator.hpp>
#include <pycom/utility/ErrorManager.hpp>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/IR/LegacyPassManager.h>

#include <llvm/Passes/PassBuilder.h>

#include <iostream>
#include <memory>

enum class CompilerState;

class Pycom
{
private:
    std::unique_ptr<llvm::LLVMContext> context;
    std::unique_ptr<llvm::IRBuilder<>> builder;
    std::unique_ptr<llvm::Module> module;

    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    llvm::PassBuilder PB;
    llvm::legacy::PassManager PM;
    std::error_code EC;

    std::string TargetTriple;
    const llvm::Target *Target;
    llvm::TargetMachine *TargetMachine;
    llvm::TargetOptions opt;
    llvm::Reloc::Model RM;

    std::unique_ptr<LexerInterface> lexer;
    std::unique_ptr<ParserInterface> parser;
    std::unique_ptr<SemanticAnalyzerInterface> seman;
    std::unique_ptr<CodeGeneratorInterface> codegen;
    std::unique_ptr<AST> ast;
    std::unique_ptr<ErrorManagerInterface> errmng;

    CompilerState state;

public:
    Pycom();
    void open(std::istream &_stream);
    AST *getAST() const;
    void checkSemantics();
    void compile();
};

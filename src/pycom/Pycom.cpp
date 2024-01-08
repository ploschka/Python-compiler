#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

#include <pycom/lib/Pycom.hpp>

enum class CompilerState
{
    created,
    file_opened,
    ast_generated,
    semantics_checked,
    compiled
};

Pycom::Pycom() : RM(llvm::Reloc::PIC_), state(CompilerState::created)
{
    errmng = std::make_unique<ErrorManager>(std::cerr);

    lexer = std::make_unique<Lexer>();
    parser = std::make_unique<Parser>();
    seman = std::make_unique<SemanticAnalyzer>();

    parser->setLexer(lexer.get());

    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeAllTargetMCs();

    TargetTriple = llvm::sys::getDefaultTargetTriple();
    std::string Error;
    Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    auto CPU = "generic";
    auto Features = "";

    TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);
}

void Pycom::open(std::istream &_stream)
{
    lexer->open(_stream);
    seman->reset();

    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("pycom", *context);

    codegen = std::make_unique<CodeGenerator>(builder.get(), module.get(), context.get());
    state = CompilerState::file_opened;
}

AST *Pycom::getAST() const
{
    return ast.get();
}

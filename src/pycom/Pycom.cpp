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
    semantics_checked,
    code_generated,
    compiled
};

Pycom::Pycom(ErrorManagerInterface *_errmng) : Target(nullptr),
                                               TargetMachine(nullptr),
                                               RM(llvm::Reloc::PIC_),
                                               errmng(_errmng),
                                               state(CompilerState::created)
{

    lexer = std::make_unique<Lexer>();
    parser = std::make_unique<Parser>();
    blockmap = std::make_unique<block_map_t>();
    seman = std::make_unique<SemanticAnalyzer>(blockmap.get());

    parser->setLexer(lexer.get());

    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeAllTargetMCs();

    TargetTriple = llvm::sys::getDefaultTargetTriple();

    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
}

void Pycom::open(std::istream &_stream)
{
    lexer->open(_stream);
    seman->reset();

    context = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*context);
    module = std::make_unique<llvm::Module>("pycom", *context);

    module->setDataLayout(TargetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);

    codegen = std::make_unique<CodeGenerator>(builder.get(), module.get(), context.get(), blockmap.get());
    ast.reset(parser->getAST());
    state = CompilerState::file_opened;
}

bool Pycom::checkSemantics()
{
    if (state >= CompilerState::file_opened)
    {
        seman->checkSemantics(ast.get());
        state = CompilerState::semantics_checked;
        bool res = true;
        return res;
    }
    errmng->error_exit("Can't check semantics, file was not opened", -1);
    return false;
}

void Pycom::generate()
{
    if (state >= CompilerState::semantics_checked)
    {
        codegen->generate(ast.get());
        state = CompilerState::code_generated;
    }
    errmng->error_exit("Can't generate code, semantics have not been checked", -1);
}

void Pycom::compile(llvm::raw_fd_ostream &_stream, llvm::OptimizationLevel _Olevel, llvm::PIELevel::Level _PIE, llvm::PICLevel::Level _PIC)
{
    if (state >= CompilerState::code_generated)
    {
        std::string Error;
        if (!Target)
        {
            Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
        }
        if (!Target)
        {
            errmng->error_exit("Couldn't create target", 1);
        }

        auto CPU = "generic";
        auto Features = "";

        TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

        module->setPIELevel(_PIE);
        module->setPICLevel(_PIC);

        // std::error_code EC;
        // llvm::raw_fd_ostream dest(_filename, EC, llvm::sys::fs::OF_None);

        // if (EC)
        // {
        //     llvm::errs() << "Could not open file: " << EC.message();
        //     exit(1);
        // }

        llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(_Olevel);

        if (llvm::verifyModule(*module, &llvm::errs()))
        {
            errmng->error_exit("Invalid module", 1);
        }
        MPM.run(*module, MAM);

        auto FileType = llvm::CGFT_ObjectFile;

        if (TargetMachine->addPassesToEmitFile(PM, _stream, nullptr, FileType))
        {
            errmng->error_exit("TargetMachine can't emit a file of this type", 1);
        }
        PM.run(*module);
        _stream.flush();
        state = CompilerState::compiled;
    }
    errmng->error_exit("Can't compile, code has not been generated", -1);
}

void Pycom::emitLLVM(llvm::raw_ostream &_stream)
{
    if (state >= CompilerState::code_generated)
    {
        module->print(_stream, nullptr);
    }
    errmng->error_exit("Can't  emit llvm, code has not been generated", -1);
}

AST *Pycom::getAST() const
{
    return ast.get();
}

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

#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>

enum class CompilerState
{
    created,
    file_opened,
    semantics_checked,
    code_generated,
    compiled,
    linked
};

Pycom::Pycom(ErrorManagerInterface *_errmng) : Target(nullptr),
                                               TargetMachine(nullptr),
                                               RM(llvm::Reloc::PIC_),
                                               errmng(_errmng),
                                               state(CompilerState::created)
{

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
    if (!Target)
    {
        Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    }
    if (!Target)
    {
        errmng->error("Couldn't create target");
    }

    auto CPU = "generic";
    auto Features = "";

    TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

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

    ast.reset(parser->getAST());
    state = CompilerState::file_opened;
}

bool Pycom::checkSemantics()
{
    if (state >= CompilerState::file_opened)
    {
        seman->checkSemantics(ast.get());
        state = CompilerState::semantics_checked;
        return true;
    }
    errmng->error("Can't check semantics, file was not opened");
    return false;
}

void Pycom::generate()
{
    if (state >= CompilerState::semantics_checked)
    {
        module->setDataLayout(TargetMachine->createDataLayout());
        module->setTargetTriple(TargetTriple);

        codegen = std::make_unique<CodeGenerator>(builder.get(), module.get(), context.get());

        codegen->generate(ast.get());
        state = CompilerState::code_generated;
    }
    else
    {
        errmng->error("Can't generate code, semantics have not been checked");
    }
}

void Pycom::compile(llvm::raw_fd_ostream &_stream, llvm::OptimizationLevel _Olevel, llvm::PIELevel::Level _PIE, llvm::PICLevel::Level _PIC)
{
    if (state >= CompilerState::code_generated)
    {
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
            errmng->error("Invalid module");
        }
        MPM.run(*module, MAM);

        auto FileType = llvm::CGFT_ObjectFile;

        if (TargetMachine->addPassesToEmitFile(PM, _stream, nullptr, FileType))
        {
            errmng->error("TargetMachine can't emit a file of this type");
        }
        PM.run(*module);
        _stream.flush();
        state = CompilerState::compiled;
    }
    else
    {
        errmng->error("Can't compile, code has not been generated");
    }
}

void Pycom::emitLLVM(llvm::raw_ostream &_stream)
{
    if (state >= CompilerState::code_generated)
    {
        module->print(_stream, nullptr);
    }
    else
    {
        errmng->error("Can't emit llvm, code has not been generated");
    }
}

void Pycom::link(std::string &_input_file, std::string &_output_file)
{
    if (state >= CompilerState::compiled)
    {
        pid_t pid = fork();
        if (pid < 0)
        {
            errmng->error("Can't create new process");
            errmng->error(strerror(errno));
            return;
        }
        if (pid == 0)
        {
            const uint32_t size = 10;

            auto args = std::make_unique<char *[]>(size);
            memset(args.get(), 0, sizeof(char *) * size);

            args[0] = "/usr/bin/ld";

            if (execvp("ld", args.get()) < 0)
            {
                errmng->error("Can't execute linker");
                errmng->error(strerror(errno));
                return;
            }
        }
        if (pid > 0)
        {
            int status;
            if (waitpid(pid, &status, 0) < 0)
            {
                errmng->error("Can't wait for linker process");
                errmng->error(strerror(errno));
            }
            if (WIFEXITED(status))
            {
                if (WEXITSTATUS(status) != 0)
                {
                    errmng->error(std::string("Linker exited with exit status ") + std::to_string(WEXITSTATUS(status)));
                }
            }
        }
        state = CompilerState::linked;
    }
    else
    {
        errmng->error("Can't link, code is not compiled");
    }
}

AST *Pycom::getAST() const
{
    return ast.get();
}

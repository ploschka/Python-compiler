#include <iostream>
#include <filesystem>
#include <fstream>

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

#include <pycom/factory/LexerFactory.hpp>
#include <pycom/factory/ParserFactory.hpp>
#include <pycom/factory/SemanticFactory.hpp>
#include <pycom/factory/CodeGenFactory.hpp>

int main(int _argc, char *_argv[])
{
    if (_argc == 1)
    {
        std::cerr << "Usage: pycom <filename>[ <filename>[...]]\n"
                     "Options:\n"
                     "-o <filename>      Set output file name\n"
                     "-c                 Only compile the file, do not link\n"
                     "-emit-llvm         Emit LLVM IR code\n"
                     "-ON                Do optimizations, N can be 0, 1, 2, 3, s, z\n";
        return 0;
    }

    for (int i = 1; i < _argc; i++)
    {
        if (!std::filesystem::exists(_argv[i]))
        {
            std::cerr << "File " << _argv[i] << " does not exist!\n";
            return -1;
        }
        if (std::filesystem::is_directory(_argv[i]))
        {
            std::cerr << _argv[i] << " is a directory!\n";
            return -1;
        }
    }

    auto lexfac = LexerFactory();
    auto parfac = ParserFactory();
    auto semfac = SemanticFactory();
    auto cdgfac = CodeGenFactory();

    std::ifstream file(_argv[1]);

    auto lexer = lexfac.create();
    lexer->open(file);
    auto parser = parfac.create();
    parser->setLexer(lexer.get());

    auto ast = parser->getAST();

    auto seman = semfac.create();

    auto context = std::make_unique<llvm::LLVMContext>();
    auto builder = std::make_unique<llvm::IRBuilder<>>(*context);
    auto module = std::make_unique<llvm::Module>("pycom", *context);

    auto codegen = cdgfac.create(builder.get(), module.get(), context.get());
    codegen->generate(ast);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    //=====================================================Компиляция=====================================================//
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    llvm::InitializeAllTargetInfos();
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::InitializeAllTargetMCs();
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);

    // Print an error and exit if we couldn't find the requested target.
    // This generally occurs if we've forgotten to initialise the
    // TargetRegistry or we have a bogus target triple.
    if (!Target)
    {
        llvm::errs() << Error;
        return 1;
    }

    auto CPU = "generic";
    auto Features = "";

    llvm::TargetOptions opt;
    llvm::Reloc::Model RM(llvm::Reloc::PIC_);
    auto TargetMachine = Target->createTargetMachine(TargetTriple, CPU, Features, opt, RM);

    module->setDataLayout(TargetMachine->createDataLayout());
    module->setTargetTriple(TargetTriple);
    module->setPIELevel(llvm::PIELevel::Level::Small);
    module->setPICLevel(llvm::PICLevel::Level::SmallPIC);

    auto output = "output.o";
    std::error_code EC;
    llvm::raw_fd_ostream dest(output, EC, llvm::sys::fs::OF_None);

    if (EC)
    {
        llvm::errs() << "Could not open file: " << EC.message();
        return 1;
    }

    // Create the analysis managers.
    llvm::LoopAnalysisManager LAM;
    llvm::FunctionAnalysisManager FAM;
    llvm::CGSCCAnalysisManager CGAM;
    llvm::ModuleAnalysisManager MAM;

    // Create the new pass manager builder.
    // Take a look at the PassBuilder constructor parameters for more
    // customization, e.g. specifying a TargetMachine or various debugging
    // options.
    llvm::PassBuilder PB;

    // Register all the basic analyses with the managers.
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);

    // Create the pass manager.
    // This one corresponds to a typical -O2 optimization pipeline.
    llvm::ModulePassManager MPM = PB.buildPerModuleDefaultPipeline(llvm::OptimizationLevel::O0);

    // Optimize the IR!
    module->print(llvm::errs(), nullptr);
    if (llvm::verifyModule(*module, &llvm::errs()))
    {
        std::cerr << "Exited\n";
        exit(-1);
    }
    MPM.run(*module, MAM);

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CGFT_ObjectFile;

    if (TargetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
    {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }
    pass.run(*module);
    dest.flush();

    return 0;
}

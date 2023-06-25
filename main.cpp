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

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/factory/LexerFactory.hpp>
#include <pycom/factory/ParserFactory.hpp>
#include <pycom/factory/SemanticFactory.hpp>
#include <pycom/factory/CodeGenFactory.hpp>

int main(int _argc, char *_argv[])
{
    if (_argc == 1)
    {
        std::cerr << "Usage: pycom <filename>[ <filename>[...]]\n";
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
    return 0;
}

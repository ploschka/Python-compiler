#include <iostream>
#include <pycom/lexer/Lexer.hpp>
#include <pycom/parser/Parser.hpp>
#include <pycom/optimizer/Optimizer.hpp>
#include "NodePrintVisitor.h"
#include <fstream>
#include <memory>
#include <pycom/utility/Types.hpp>
#include <pycom/semanalyzer/SemanticAnalyzer.hpp>
#include <pycom/error_manager/ThrowErrorManager.hpp>
#include <pycom/codegen/CodeGenerator.hpp>

int main()
{
    std::ifstream file("scripts/lexer.py");
    try
    {
        auto lexer = std::make_unique<Lexer>();
        lexer->open(file);
        auto parser = std::make_unique<Parser>();
        parser->setLexer(lexer.get());
        auto visitor = std::make_unique<PrintVisitor>();
        auto seman = std::make_unique<SemanticAnalyzer>();
        seman->setEM(new ThrowErrorManager());
        auto codegen = std::make_unique<CodeGenerator>(std::cout);

        auto ast = parser->getAST();
        seman->checkSemantics(ast);

        std::cout << "Befor optimization:\n\n";
        codegen->generate(ast);

        std::cout << "\n\nAfter optimization:\n\n";
        auto optimizer = std::make_unique<Optimizer>();
        optimizer->optimize(ast);
        codegen->generate(ast);
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}

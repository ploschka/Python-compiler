#include <iostream>
#include <pycom/lexer/Lexer.hpp>
#include <pycom/parser/Parser.hpp>
#include <pycom/optimizer/Optimizer.hpp>
#include "NodePrintVisitor.h"
#include <fstream>
#include <memory>
#include <pycom/utility/Types.hpp>
#include <pycom/semanalyzer/SemanticAnalyzer.hpp>
#include "ThrowErrorManager.hpp"

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

        auto ast = parser->getAST();
        seman->checkSemantics(ast);

        std::cout << "Befor optimization:\n\n";
        ast->accept(visitor.get());

        std::cout << "\n\nAfter optimization:\n\n";
        auto optimizer = std::make_unique<Optimizer>();
        optimizer->optimize(ast);
        ast->accept(visitor.get());
    }
    catch (std::exception &e)
    {
        std::cout << e.what();
    }
    return 0;
}

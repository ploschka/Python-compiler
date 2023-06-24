#include <iostream>
#include <pycom/factory/LexerFactory.hpp>
#include <pycom/factory/ParserFactory.hpp>
#include <pycom/example_dependencies/NodePrintVisitor.h>
#include <fstream>
#include <memory>

int main()
{
    auto lexfac = LexerFactory();
    auto parfac = ParserFactory();
    std::ifstream file("scripts/lexer.py");

    auto lexer = lexfac.create();
    lexer->open(file);
    auto parser = parfac.create();
    parser->setLexer(lexer.get());
    auto visitor = std::make_unique<PrintVisitor>();
    parser->getAST()->accept(visitor.get());

    return 0;
}

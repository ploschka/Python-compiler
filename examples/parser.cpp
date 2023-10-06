#include <iostream>
#include <pycom/lexer/Lexer.hpp>
#include <pycom/parser/Parser.hpp>
#include "NodePrintVisitor.h"
#include <fstream>
#include <memory>

int main()
{
    std::ifstream file("scripts/lexer.py");

    auto lexer = std::make_unique<Lexer>();
    lexer->open(file);
    auto parser = std::make_unique<Parser>();
    parser->setLexer(lexer.get());
    auto visitor = std::make_unique<PrintVisitor>();
    parser->getAST()->accept(visitor.get());

    return 0;
}

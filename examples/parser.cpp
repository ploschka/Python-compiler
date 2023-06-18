#include <iostream>
#include <pycom/factory/LexerFactory.hpp>
#include <pycom/factory/ParserFactory.hpp>
#include <fstream>

int main()
{
    auto lexfac = LexerFactory();
    auto parfac = ParserFactory();
    std::ifstream file("scripts/lexer.py");

    auto lexer = lexfac.create();
    lexer->open(file);
    auto parser = parfac.create();
    parser->setLexer(lexer.get());
    parser->getAST()->print();

    return 0;
}

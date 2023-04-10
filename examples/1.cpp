#include <pycom/factory/LexerFactory.hpp>
#include <iostream>

int main()
{
    auto lexfac = LexerFactory();
    auto lexer = lexfac.create();

    lexer->openFile("/scripts/1.py");
    for (int i = 0; i < 20; i++)
    {
        auto lexeme = lexer->getLexeme();
        std::cout << lexeme.getValue() << ' ' << static_cast<int>(lexeme.getType()) << ' ' << lexeme.getRow() <<  ' ' << lexeme.getPos() << std::endl;
    }
}

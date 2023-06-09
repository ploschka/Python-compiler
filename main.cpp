#include <iostream>
#include <filesystem>
#include <pycom/factory/LexerFactory.hpp>
#include <pycom/factory/ParserFactory.hpp>
#include <pycom/factory/SemanticFactory.hpp>
#include <pycom/factory/CodeGenFactory.hpp>

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        std::cerr << "Usage: pycom <filename>[ <filename>[...]]\n";
        return 0;
    }

    for (int i = 1; i < argc; i++)
    {
        if (!std::filesystem::exists(argv[i]))
        {
            std::cerr << "File " << argv[i] << " does not exist!\n";
            return -1;
        }
        if (std::filesystem::is_directory(argv[i]))
        {
            std::cerr << argv[i] << " is a directory!\n";
            return -1;
        }
    }

    auto lexfac = LexerFactory();
    auto parfac = ParserFactory();
    auto semfac = SemanticFactory();
    auto cdgfac = CodeGenFactory();

    auto lexer = lexfac.create();
    auto parser = parfac.create();
    parser->setLexer(lexer.get());
    auto seman = semfac.create();
    auto codegen = cdgfac.create();
    return 0;
}

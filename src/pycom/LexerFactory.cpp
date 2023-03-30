#include <pycom/factory/LexerFactory.hpp>
#include <pycom/lexer/Lexer.hpp>

std::unique_ptr<LexerInterface> LexerFactory::create() {
    return std::make_unique<Lexer>();
}

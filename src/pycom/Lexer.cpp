#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>
#define CHARCOUNT 1000

Type Lexer::recognize() const
{
    return Type();
}

bool Lexer::openFile(std::string filename)
{
    file.open(filename);
    auto op = file.is_open();
    if (op) {
        buffer1.clear();
        buffer2.clear();

        buffer1.append(CHARCOUNT, '\0');
        file.read(&buffer1[0], CHARCOUNT);
        eof = file.eof();
    }
    return op;
}

void Lexer::setState(LexerStateInterface* state)
{
    this->state.reset(state);
}

Lexeme Lexer::getLexeme()
{
    if (file.is_open()) {
        std::string str(CHARCOUNT, '\0');
        file.read(&str[0], CHARCOUNT);
    }
    return Lexeme("eof", Type::eof);
}

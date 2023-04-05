#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>
#define CHARCOUNT 1000

Type Lexer::recognize(const std::string& id) const
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

        buffer1.append(CHARCOUNT + 1, '\0');
        file.read(&buffer1[0], CHARCOUNT);
        iter = buffer1.cbegin();
        currBuff = &buffer1;
        otherBuff = &buffer2;
        type = Type::eof;
        row = 1;
        pos = 1;
        setState(new Start(this, this->accum, this->type, this->row, this->pos));
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
        while (!(this->state->recognize(*iter++)) &&
              iter != currBuff->cend());
        
        if (type == Type::eof) {
            if (iter == currBuff->cend()) {
                otherBuff->clear();
                otherBuff->append(CHARCOUNT + 1, '\0');
                file.read(&(*otherBuff)[0], CHARCOUNT);
                iter = otherBuff->cbegin();
                std::swap(currBuff, otherBuff);
                while (!(this->state->recognize(*iter++)) &&
                    iter != currBuff->cend());
                if (type == Type::eof)
                    return Lexeme("eof", Type::eof);
            } else
                Lexeme("eof", Type::eof, row, pos);
        }
        if (type == Type::id)
            return Lexeme(accum, recognize(accum), row, pos);
        else
            return Lexeme(accum, type);
    }
    return Lexeme("eof", Type::eof);
}

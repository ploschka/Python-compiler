#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <iostream>
#define CHARCOUNT 1000

Type Lexer::recognize(const std::string &id) const
{
    static const std::unordered_map<std::string, Type> map = {
        {"def", Type::def},
        {"if", Type::ifop},
        {"elif", Type::elifop},
        {"else", Type::elseop},
        {"for", Type::forop},
        {"while", Type::whileop},
        {"class", Type::classop}};

    if (map.find(id) != map.end())
        return map.at(id);
    else
        return Type::id;
}

bool Lexer::openFile(std::string filename)
{
    file.open(filename);
    auto op = file.is_open();
    if (op)
    {
        buffer1.clear();
        buffer2.clear();

        buffer1.append(CHARCOUNT + 1, '\0');
        file.read(&buffer1[0], CHARCOUNT);
        iter = buffer1.cbegin();
        currBuff = &buffer1;
        otherBuff = &buffer2;
        type = Type::eof;
        row = 1;
        pos = 0;
        setState(new Start(this, this->accum, this->type, this->row, this->pos));
    }
    return op;
}

void Lexer::setState(LexerStateInterface *state)
{
    this->state.reset(state);
}

Lexeme Lexer::getLexeme()
{
    if (file.is_open())
    {
        while (!(this->state->recognize(*iter++)) &&
               iter != currBuff->cend())
            ;

        if (type == Type::eof)
        {
            if (iter == currBuff->cend())
            {
                otherBuff->clear();
                otherBuff->append(CHARCOUNT + 1, '\0');
                file.read(&(*otherBuff)[0], CHARCOUNT);
                iter = otherBuff->cbegin();
                std::swap(currBuff, otherBuff);
                while (!(this->state->recognize(*iter++)) &&
                       iter != currBuff->cend())
                    ;
                if (type == Type::eof)
                {
                    return Lexeme(Type::eof, row, pos);
                    file.close();
                }
            }
            else
            {
                Lexeme(Type::eof, row, pos);
                file.close();
            }
        }
        Type retType;
        if (type == Type::id)
            retType = recognize(accum);
        else
            retType = type;

        auto retAcc = accum;
        accum.assign("");
        return Lexeme(retAcc, retType, row, pos);
    }
    return Lexeme(Type::eof);
}

#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>

#include <unordered_map>
#include <iostream>

#define CHARCOUNT 1000

Type Lexer::recognize(const std::string &_id) const
{
    static const std::unordered_map<std::string, Type> map = {
        {"def", Type::defkw},
        {"if", Type::ifkw},
        {"elif", Type::elifkw},
        {"else", Type::elsekw},
        {"for", Type::forkw},
        {"while", Type::whilekw},
        {"class", Type::classkw},
        {"return", Type::returnkw},
        {"yield", Type::yieldkw},
        {"continue", Type::continuekw},
        {"break", Type::breakkw},
        {"and", Type::andop},
        {"or", Type::orop},
        {"not", Type::notop},
        {"in", Type::in},
        {"is", Type::is},
        {"pass", Type::passkw}
    };

    if (map.find(_id) != map.end())
        return map.at(_id);
    else
        return Type::id;
}

void Lexer::open(std::istream &_stream)
{
    lexerdata = std::make_unique<LexerData>();
    currBuff = std::make_unique<std::string>(CHARCOUNT + 1, '\0');
    otherBuff = std::make_unique<std::string>();

    this->stream = &_stream;

    this->stream->read(&(*currBuff)[0], CHARCOUNT);
    iter = currBuff->cbegin();
    setState(new Start(this, lexerdata.get()));
}

void Lexer::setState(LexerStateInterface *_state)
{
    this->state.reset(_state);
}

Token Lexer::getToken()
{
    while (lexerdata->queue.empty() && iter != currBuff->cend())
    {
        this->state->recognize(*iter++);
    }

    Token tok = lexerdata->get();
    bool failed = false;

    if (tok.getType() == Type::eof)
    {
        if (iter == currBuff->cend())
        {
            otherBuff->assign(CHARCOUNT + 1, '\0');
            stream->read(&(*otherBuff)[0], CHARCOUNT);
            iter = otherBuff->cbegin();
            currBuff.swap(otherBuff);
            while (lexerdata->queue.empty() && iter != currBuff->cend())
            {
                this->state->recognize(*iter++);
            }
            tok = lexerdata->get();
            if (tok.getType() == Type::eof)
            {
                failed = true;
            }
        }
        else
        {
            failed = true;
        }
    }
    if (failed)
    {
        lexerdata->unwind();
        lexerdata->queue.push(tok);
        tok = lexerdata->get();
        return tok;
    }

    if (tok.getType() == Type::id)
    {
        tok.setType(recognize(tok.getValue()));
    }
    return tok;
}

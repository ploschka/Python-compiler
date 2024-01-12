#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>

#include <unordered_map>
#include <iostream>
#include <cstring>

const size_t CHARCOUNT = 1023;

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
        {"pass", Type::passkw}};

    if (map.find(_id) != map.end())
        return map.at(_id);
    else
        return Type::id;
}

void Lexer::open(std::istream &_stream)
{
    lexerdata = std::make_unique<LexerData>();
    if (!currBuff || !otherBuff)
    {
        currBuff.reset(new char[CHARCOUNT + 1]);
        otherBuff.reset(new char[CHARCOUNT + 1]);
    }
    memset(currBuff.get(), 0, CHARCOUNT + 1);

    this->stream = &_stream;

    this->stream->read(currBuff.get(), CHARCOUNT);
    iter = currBuff.get();
    setState(std::make_unique<Start>(this, lexerdata.get()));
}

void Lexer::setState(std::unique_ptr<LexerStateInterface> &&_state)
{
    this->state.reset(_state.release());
}

Token Lexer::getToken()
{
    while (lexerdata->queue.empty() && (iter - currBuff.get() != (CHARCOUNT + 1)))
    {
        this->state->recognize(*iter++);
    }

    Token tok = lexerdata->get();
    bool failed = false;

    if (tok.getType() == Type::eof)
    {
        if (iter - currBuff.get() == (CHARCOUNT + 1))
        {
            memset(otherBuff.get(), 0, CHARCOUNT + 1);
            stream->read(otherBuff.get(), CHARCOUNT);
            iter = otherBuff.get();
            currBuff.swap(otherBuff);
            while (lexerdata->queue.empty() && (iter - currBuff.get() != (CHARCOUNT + 1)))
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

#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <iostream>
#define CHARCOUNT 1000

static void eraseQueue(tokenQueue_t &q)
{
    while (!q.empty())
    {
        q.pop();
    }
}
extern void unwind(LexerInterface *lexer);

Type Lexer::recognize(const std::string &id) const
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
        {"break", Type::breakkw}};

    if (map.find(id) != map.end())
        return map.at(id);
    else
        return Type::id;
}

void Lexer::open(std::istream &stream)
{
    buffer1.assign(CHARCOUNT + 1, '\0');
    buffer2.clear();
    this->stream = &stream;
    eraseQueue(queue);

    this->stream->read(&buffer1[0], CHARCOUNT);
    iter = buffer1.cbegin();
    currBuff = &buffer1;
    otherBuff = &buffer2;
    setState(new Start(this));
}

void Lexer::setState(LexerStateInterface *state)
{
    this->state.reset(state);
}

Token Lexer::getToken()
{
    while (queue.empty() && iter != currBuff->cend())
    {
        this->state->recognize(*iter++);
    }

    auto tok = queue.front();
    queue.pop();

    bool failed = false;

    if (tok.getType() == Type::eof)
    {
        if (iter == currBuff->cend())
        {
            otherBuff->assign(CHARCOUNT + 1, '\0');
            stream->read(&(*otherBuff)[0], CHARCOUNT);
            iter = otherBuff->cbegin();
            std::swap(currBuff, otherBuff);
            while (queue.empty() && iter != currBuff->cend())
            {
                this->state->recognize(*iter++);
            }
            tok = queue.front();
            queue.pop();
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
        unwind(this);
        queue.push(tok);
        tok = queue.front();
        queue.pop();
        return tok;
    }

    if (tok.getType() == Type::id)
    {
        tok.setType(recognize(tok.getValue()));
    }
    return tok;
}

void Lexer::pushToQueue(Token token)
{
    queue.push(token);
}

#include <pycom/lexer/Lexer.hpp>
#include <pycom/lexer/LexerStates.hpp>
#include <unordered_map>
#include <iostream>
#define CHARCOUNT 1000

void eraseQueue(tokenQueue_t& q)
{
    while(!q.empty())
    {
        q.pop();
    }
}

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

void Lexer::open(std::istream& stream)
{
    buffer1.clear();
    buffer2.clear();
    this->stream = &stream;
    eraseQueue(queue);

    buffer1.append(CHARCOUNT + 1, '\0');
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

LexerStateInterface *Lexer::getState()
{
    return state.release();
}

Token Lexer::getToken()
{
    unsigned int position;
    while (!(position = this->state->recognize(*iter++)) &&
            iter != currBuff->cend())
        ;

    if (type == Type::eof)
    {
        if (iter == currBuff->cend())
        {
            otherBuff->clear();
            otherBuff->append(CHARCOUNT + 1, '\0');
            stream->read(&(*otherBuff)[0], CHARCOUNT);
            iter = otherBuff->cbegin();
            std::swap(currBuff, otherBuff);
            while (!(this->state->recognize(*iter++)) &&
                    iter != currBuff->cend())
                ;
            if (type == Type::eof)
            {
                return Token(Type::eof, row, position);
            }
        }
        else
        {
            Token(Type::eof, row, position);
        }
    }
    Type retType;
    std::string retAcc;
    if (type == Type::id || type == Type::number || type == Type::string)
    {
        retAcc = accum;
        accum.assign("");
    }
    if (type == Type::id)
    {
        retType = recognize(retAcc);
    }
    else
        retType = type;
    type = Type::eof;
    return Token(retAcc, retType, row, position);
}

unsigned int& Lexer::getRow()
{
    return row;
}

unsigned int& Lexer::getPos()
{
    return pos;
}

void Lexer::pushToQueue(Token token)
{
    queue.push(token);
}

Token Lexer::popFromQueue(Token token)
{
    auto ret = queue.back();
    queue.pop();
    return ret;
}

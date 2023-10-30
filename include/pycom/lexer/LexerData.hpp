#pragma once

#include <pycom/token/Token.hpp>

#include <stack>
#include <string>
#include <queue>

enum class IndentType
{
    null,
    space,
    tab
};


struct LexerData
{
    typedef std::stack<unsigned int> instack_t;
    typedef std::queue<Token> tokenQueue_t;
    unsigned int pos = 1;
    unsigned int row = 1;
    instack_t stack;
    std::string accum;
    IndentType intype;
    tokenQueue_t queue;
    void unwind();
    Token get();
    void put(Type _type, unsigned int _row = 0, unsigned int _pos = 0);
    LexerData();
};

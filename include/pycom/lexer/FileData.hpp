#pragma once
#include <stack>
#include <string>
#include <queue>
#include <pycom/token/Token.hpp>

enum class IndentType
{
    null,
    space,
    tab
};

typedef std::stack<unsigned int> instack_t;
typedef std::queue<Token> tokenQueue_t;

struct FileData
{
    unsigned int pos = 1;
    unsigned int row = 1;
    instack_t stack;
    std::string accum;
    IndentType intype;
    tokenQueue_t queue;
    void unwind();
    Token get();
    void put(Type _type, unsigned int _row = 0, unsigned int _pos = 0);
    FileData();
};

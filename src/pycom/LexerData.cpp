#include <pycom/lexer/LexerData.hpp>

void LexerData::unwind()
{
    while (stack.top() != 0)
    {
        queue.emplace("", Type::dedent, row, pos);
        stack.pop();
    }
}

LexerData::LexerData()
{
    stack.push(0);
    intype = IndentType::null;
}

Token LexerData::get()
{
    Token tok = queue.front();
    queue.pop();
    return tok;
}

void LexerData::put(Type _type, unsigned int _row, unsigned int _pos)
{
    queue.emplace(accum, _type, _row, _pos);
    accum.clear();
}

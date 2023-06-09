#include <pycom/lexer/FileData.hpp>

void FileData::unwind()
{
    while (stack.top() != 0)
    {
        queue.emplace("", Type::dedent, row, pos);
        stack.pop();
    }
}

FileData::FileData()
{
    stack.push(0);
    intype = IndentType::null;
}

Token FileData::get()
{
    Token tok = queue.front();
    queue.pop();
    return tok;
}

void FileData::put(Type _type, unsigned int _row, unsigned int _pos)
{
    queue.emplace(accum, _type, _row, _pos);
    accum.clear();
}

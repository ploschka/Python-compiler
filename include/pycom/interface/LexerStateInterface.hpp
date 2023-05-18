#pragma once

class LexerStateInterface
{
public:
    virtual bool recognize(char ch) = 0;
    virtual ~LexerStateInterface() = default;
};

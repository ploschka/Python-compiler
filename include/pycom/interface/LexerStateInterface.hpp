#pragma once

class LexerStateInterface
{
public:
    virtual bool recognize(char _ch) = 0;
    virtual ~LexerStateInterface() = default;
};

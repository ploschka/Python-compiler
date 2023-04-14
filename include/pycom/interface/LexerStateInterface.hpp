#pragma once

class LexerStateInterface
{
public:
    virtual unsigned int recognize(char ch) = 0;
    virtual ~LexerStateInterface() = default;
};

#pragma once

class LexerStateInterface
{
public:
    virtual void recognize() = 0;
    virtual ~LexerStateInterface() = default;
};

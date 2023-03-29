#pragma once
#include "pycom/interface/LexemeInterface.hpp"

class LexerInterface
{
public:
    virtual int openFile(std::string filename) = 0;
    virtual LexemeInterface getLexeme() = 0;
};

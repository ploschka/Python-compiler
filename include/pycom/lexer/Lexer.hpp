#pragma once

#include <pycom/interface/LexerInterface.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/lexer/LexerData.hpp>

#include <memory>
#include <queue>

class Lexer : public LexerInterface
{
private:
    std::istream *stream;
    std::unique_ptr<char> currBuff;
    std::unique_ptr<char> otherBuff;
    std::unique_ptr<LexerStateInterface> state;
    char *iter;
    std::unique_ptr<LexerData> lexerdata;

    Type recognize(const std::string &_id) const;

public:
    void open(std::istream &_stream);
    void setState(std::unique_ptr<LexerStateInterface> &&_state);
    Token getToken();
};

#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/token/Token.hpp>
#include <memory>
#include <queue>

typedef std::queue<Token> tokenQueue_t;

class Lexer: public LexerInterface
{
private:
    std::istream* stream;
    std::string buffer1;
    std::string buffer2;
    std::string* currBuff;
    std::string* otherBuff;
    std::unique_ptr<LexerStateInterface> state;
    std::string::const_iterator iter;
    tokenQueue_t queue;

    Type recognize(const std::string& id) const;
public:
    void open(std::istream& stream);
    void setState(LexerStateInterface* state);
    Token getToken();
    void pushToQueue(Token token);
};

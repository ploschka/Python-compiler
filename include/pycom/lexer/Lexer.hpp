#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/token/Token.hpp>
#include <fstream>
#include <memory>
#include <stack>
#include <queue>

enum class IndentType;
typedef std::stack<unsigned int> instack;
typedef std::queue<Token> tokenQueue_t;


class Lexer: public LexerInterface
{
private:
    std::ifstream file;
    std::string buffer1;
    std::string buffer2;
    std::string* currBuff;
    std::string* otherBuff;
    std::string accum;
    Type type;
    std::unique_ptr<LexerStateInterface> state;
    std::string::const_iterator iter;
    unsigned int row;
    unsigned int pos;
    instack indentStack;
    IndentType indentType;
    tokenQueue_t queue;

    Type recognize(const std::string& id) const;
public:
    bool openFile(std::string filename);
    void setState(LexerStateInterface* state);
    LexerStateInterface* getState();
    Token getToken();
    unsigned int& getRow();
    unsigned int& getPos();
    void pushToQueue(Token token);
    Token popFromQueue(Token token);
};

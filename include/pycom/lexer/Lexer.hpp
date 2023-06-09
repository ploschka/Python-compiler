#pragma once
#include <pycom/interface/LexerInterface.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/lexer/FileData.hpp>
#include <memory>
#include <queue>

class Lexer: public LexerInterface
{
private:
    std::istream* stream;
    std::unique_ptr<std::string> currBuff;
    std::unique_ptr<std::string> otherBuff;
    std::unique_ptr<LexerStateInterface> state;
    std::string::const_iterator iter;
    std::unique_ptr<FileData> filedata;

    Type recognize(const std::string& _id) const;
public:
    void open(std::istream& _stream);
    void setState(LexerStateInterface* _state);
    Token getToken();
};

#include <vector>
#include <pycom/interface/LexerInterface.hpp>


class LexerMock : public LexerInterface
{
private:
    std::vector<Token> tokens;
    int i;

public:
    LexerMock(std::vector<Token> tokens);
    Token getToken();

    void open(std::istream &_stream) {}
    void setState(LexerStateInterface *_state) {}
};
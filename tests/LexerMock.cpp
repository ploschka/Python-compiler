#include <pycom/lexer/LexerMock.hpp>

LexerMock::LexerMock(std::vector<Token> tokens) {
    this->tokens = tokens;
    this->i = 0;
}

Token LexerMock::getToken() {
    if (this->i >= this->tokens.size()) {
        return Token("", Type::eof, 0, 0);
    } else {
        return this->tokens[this->i++];
    }
}

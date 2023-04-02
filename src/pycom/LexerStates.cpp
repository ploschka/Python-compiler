#include <pycom/lexer/LexerStates.hpp>

bool SomeLexerState::recognize(char ch) {
    int a = 2 + 2;
    return true;
}

bool OtherLexerState::recognize(char ch) {
    lexer->setState(new SomeLexerState());
    return true;
}

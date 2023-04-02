#include <pycom/lexer/LexerStates.hpp>

void SomeLexerState::recognize() {
    int a = 2 + 2;
}

void OtherLexerState::recognize() {
    lexer->setState(new SomeLexerState());
}

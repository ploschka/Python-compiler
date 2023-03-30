#include "pycom/lexer/Lexer.hpp"
#include <fstream>
#define CHARCOUNT 20

bool Lexer::openFile(std::string filename) {
    file.open(filename);
    return file.is_open();
}
Lexeme Lexer::getLexeme() const {
    std::string str(CHARCOUNT, ' ');
    return Lexeme();
}

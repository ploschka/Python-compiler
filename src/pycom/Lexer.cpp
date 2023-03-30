#include "pycom/lexer/Lexer.hpp"
#include <fstream>

bool Lexer::openFile(std::string filename) {
    file.open(filename);
    return file.is_open();
}
Lexeme Lexer::getLexeme() const {
    
}

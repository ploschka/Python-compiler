#pragma once
#include <string>

/// @brief Тип токена
enum class Type {
    id,         // Любой любое слово, позже оно будет распознано как ключевое слово или реально как идентификатор
    number,     // Любое число, на последующих этапах трансляции будет распознано как целое или вещественное
    string,     // Строка
    lpr,        // Символ (
    rpr,        // Символ )
    lsbr,       // Символ [
    rsbr,       // Символ ]
    lbr,        // Символ {
    rbr,        // Символ }
    plus,       // Символ +
    minus,      // Символ -
    star,       // Символ *
    slash,      // Символ /
    dot,        // Символ .
    great,      // Символ >
    less,       // Символ <
    equal,      // Символ ==
    noteq,      // Символ !=
    grequal,    // Символ >=
    lequal,     // Символ <=
    notop,      // Ключевое слово not
    in,         // Ключевое слово in
    assign,     // Символ =
    def,        // Ключевое слово def
    ifop,       // Ключевое слово if
    elifop,     // Ключевое слово elif
    forop,      // Ключевое слово for
    whileop,    // Ключевое слово while
    colon,      // Символ :
    indent,     // Eсли отступ от края больше, чем у предыдущей строки
    dedent,     // Eсли отступ от края меньше, чем у предыдущей строки
    newline,    // Новая строка
    eof         // Конец файла
};

class Lexeme
{
private:
    std::string value;
    Type type;
public:
    std::string getValue() const;
    Type getType() const;
    void setValue(std::string value);
    void setType(Type type);
    Lexeme() = default;
    Lexeme(std::string value, Type type);
};

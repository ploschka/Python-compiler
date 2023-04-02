#pragma once
#include <string>

/// @brief Тип токена
enum class Type {
    id,         // Любой любое слово, позже оно будет распознано как ключевое слово, оператор или реально как идентификатор
    number,     // Любое число, на последующих этапах трансляции будет распознано как целое или вещественное
    string,     // Строка
    lpr,        // Символ (
    rpr,        // Символ )
    lsbr,       // Символ [
    rsbr,       // Символ ]
    lbr,        // Символ {
    rbr,        // Символ }
    colon,      // Символ :
    plus,       // Оператор +
    minus,      // Оператор -
    star,       // Оператор *
    div,        // Оператор /
    dot,        // Оператор .
    greater,    // Оператор >
    less,       // Оператор <
    equal,      // Оператор ==
    noteq,      // Оператор !=
    grequal,    // Оператор >=
    lequal,     // Оператор <=
    notop,      // Оператор not
    in,         // Оператор in
    is,         // Оператор is
    assign,     // Оператор =
    plusass,    // Оператор +=
    minass,     // Оператор -=
    mulass,     // Оператор *=
    divass,     // Оператор /=
    def,        // Ключевое слово def
    ifop,       // Ключевое слово if
    elifop,     // Ключевое слово elif
    elseop,     // Ключевое слово else
    forop,      // Ключевое слово for
    whileop,    // Ключевое слово while
    class,      // Ключевое слово class
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

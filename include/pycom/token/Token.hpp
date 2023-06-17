#pragma once
#include <string>

/// @brief Тип токена
enum class Type {
    id,         // Идентификатор
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
    mod,        // Оператор %
    idiv,       // Оператор //
    matmul,     // Оператор @
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
    andop,      // Оператор and
    orop,       // Оператор or
    inv,        // Оператор ~
    lshift,     // Оператор <<
    rshift,     // Оператор >>
    band,       // Оператор &
    bor,        // Оператор |
    xorop,      // Оператор ^
    assign,     // Оператор =
    plusass,    // Оператор +=
    minass,     // Оператор -=
    mulass,     // Оператор *=
    divass,     // Оператор /=
    modass,     // Оператор %=
    idivass,    // Оператор //=
    matmulass,  // Оператор @=
    lshiftass,  // Оператор <<=
    rshiftass,  // Оператор >>=
    bandass,    // Оператор &=
    borass,     // Оператор |=
    xorass,     // Оператор ^=
    arrow,      // Оператор ->
    defkw,      // Ключевое слово def
    ifkw,       // Ключевое слово if
    elifkw,     // Ключевое слово elif
    elsekw,     // Ключевое слово else
    forkw,      // Ключевое слово for
    whilekw,    // Ключевое слово while
    classkw,    // Ключевое слово class
    yieldkw,    // Ключевое слово yield
    returnkw,   // Ключевое слово return
    continuekw, // Ключевое слово continue
    passkw,     // Ключевое слово pass
    breakkw,    // Ключевое слово break
    indent,     // Eсли отступ от края больше, чем у предыдущей строки
    dedent,     // Eсли отступ от края меньше, чем у предыдущей строки
    newline,    // Новая строка
    eof,        // Конец файла
    comma,      // Запятая
    unexpected, // Без комментариев
    indenterror,// Ошибка отступов
    tabspacemix // Смесь табов и пробелов в отступах
};

std::string type_to_str(Type type);

class Token
{
private:
    std::string value;
    Type type;
    unsigned int row;
    unsigned int pos;

public:
    std::string getValue() const;
    Type getType() const;
    unsigned int getPos() const;
    unsigned int getRow() const;
    void setValue(std::string _value);
    void setType(Type _type);
    Token(std::string _value, Type _type, unsigned int _row = 0, unsigned int _pos = 0);
    bool operator==(const Token& _other) const;
};

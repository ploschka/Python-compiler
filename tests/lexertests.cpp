#include <pycom/lexer/Lexer.hpp>
#include <gtest/gtest.h>
#include <vector>
#include <fstream>
#include <sstream>

static std::unique_ptr<LexerInterface> lexer = std::make_unique<Lexer>();

static bool compvec(const std::vector<Token>& _v1, const std::vector<Token>& _v2)
{
    if (_v1.size() != _v2.size())
    {
        return false;
    }
    auto iter1 = _v1.cbegin();
    auto iter2 = _v2.cbegin();
    while (iter1 != _v1.cend() && iter2 != _v2.cend())
    {
        if (*iter1 != *iter2)
        {
            return false;
        }
        ++iter1;
        ++iter2;
    }
    return true;
}

TEST(LexerTest, IdLettersOnly)
{
    std::stringstream ss1;
    ss1 << "folga";
    lexer->open(ss1);
    Token tok1("folga", Type::id, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "abba";
    lexer->open(ss2);
    Token tok2("abba", Type::id, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "aa";
    lexer->open(ss3);
    Token tok3("aa", Type::id, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "a";
    lexer->open(ss4);
    Token tok4("a", Type::id, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());
}

TEST(LexerTest, IdLettersAndNumbers)
{
    std::stringstream ss1;
    ss1 << "folga123";
    lexer->open(ss1);
    Token tok1("folga123", Type::id, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "folg123a";
    lexer->open(ss2);
    Token tok2("folg123a", Type::id, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "f123olga";
    lexer->open(ss3);
    Token tok3("f123olga", Type::id, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "fol1ga";
    lexer->open(ss4);
    Token tok4("fol1ga", Type::id, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());   
}

TEST(LexerTest, IdUnderscore)
{
    std::stringstream ss1;
    ss1 << "folga_";
    lexer->open(ss1);
    Token tok1("folga_", Type::id, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "folga__";
    lexer->open(ss2);
    Token tok2("folga__", Type::id, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "fol__ga";
    lexer->open(ss3);
    Token tok3("fol__ga", Type::id, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "__folga";
    lexer->open(ss4);
    Token tok4("__folga", Type::id, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());   
}

TEST(LexerTest, NumberInt)
{
    std::stringstream ss1;
    ss1 << "221";
    lexer->open(ss1);
    Token tok1("221", Type::number, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "2281";
    lexer->open(ss2);
    Token tok2("2281", Type::number, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "0";
    lexer->open(ss3);
    Token tok3("0", Type::number, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "1234567034567890123456789876543234567";
    lexer->open(ss4);
    Token tok4("1234567034567890123456789876543234567", Type::number, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());   
}

TEST(LexerTest, NumberFloat)
{
    std::stringstream ss1;
    ss1 << "22.1";
    lexer->open(ss1);
    Token tok1("22.1", Type::number, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "2.281";
    lexer->open(ss2);
    Token tok2("2.281", Type::number, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "0.0";
    lexer->open(ss3);
    Token tok3("0.0", Type::number, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "0.1234567034567890123456789876543234567";
    lexer->open(ss4);
    Token tok4("0.1234567034567890123456789876543234567", Type::number, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());   
}

TEST(LexerTest, Keyword)
{
    std::stringstream ss1;
    ss1 << "def";
    lexer->open(ss1);
    Token tok1("def", Type::defkw, 1, 1);
    ASSERT_EQ(tok1, lexer->getToken());

    std::stringstream ss2;
    ss2 << "if";
    lexer->open(ss2);
    Token tok2("if", Type::ifkw, 1, 1);
    ASSERT_EQ(tok2, lexer->getToken());

    std::stringstream ss3;
    ss3 << "elif";
    lexer->open(ss3);
    Token tok3("elif", Type::elifkw, 1, 1);
    ASSERT_EQ(tok3, lexer->getToken());

    std::stringstream ss4;
    ss4 << "else";
    lexer->open(ss4);
    Token tok4("else", Type::elsekw, 1, 1);
    ASSERT_EQ(tok4, lexer->getToken());

    std::stringstream ss5;
    ss5 << "for";
    lexer->open(ss5);
    Token tok5("for", Type::forkw, 1, 1);
    ASSERT_EQ(tok5, lexer->getToken());

    std::stringstream ss6;
    ss6 << "while";
    lexer->open(ss6);
    Token tok6("while", Type::whilekw, 1, 1);
    ASSERT_EQ(tok6, lexer->getToken());

    std::stringstream ss7;
    ss7 << "class";
    lexer->open(ss7);
    Token tok7("class", Type::classkw, 1, 1);
    ASSERT_EQ(tok7, lexer->getToken());

    std::stringstream ss8;
    ss8 << "yield";
    lexer->open(ss8);
    Token tok8("yield", Type::yieldkw, 1, 1);
    ASSERT_EQ(tok8, lexer->getToken());

    std::stringstream ss9;
    ss9 << "return";
    lexer->open(ss9);
    Token tok9("return", Type::returnkw, 1, 1);
    ASSERT_EQ(tok9, lexer->getToken());

    std::stringstream ss10;
    ss10 << "continue";
    lexer->open(ss10);
    Token tok10("continue", Type::continuekw, 1, 1);
    ASSERT_EQ(tok10, lexer->getToken());

    std::stringstream ss11;
    ss11 << "pass";
    lexer->open(ss11);
    Token tok11("pass", Type::passkw, 1, 1);
    ASSERT_EQ(tok11, lexer->getToken());

    std::stringstream ss12;
    ss12 << "break";
    lexer->open(ss12);
    Token tok12("break", Type::breakkw, 1, 1);
    ASSERT_EQ(tok12, lexer->getToken());

    std::stringstream ss13;
    ss13 << "not";
    lexer->open(ss13);
    Token tok13("not", Type::notop, 1, 1);
    ASSERT_EQ(tok13, lexer->getToken());

    std::stringstream ss14;
    ss14 << "in";
    lexer->open(ss14);
    Token tok14("in", Type::in, 1, 1);
    ASSERT_EQ(tok14, lexer->getToken());

    std::stringstream ss15;
    ss15 << "is";
    lexer->open(ss15);
    Token tok15("is", Type::is, 1, 1);
    ASSERT_EQ(tok15, lexer->getToken());

    std::stringstream ss16;
    ss16 << "and";
    lexer->open(ss16);
    Token tok16("and", Type::andop, 1, 1);
    ASSERT_EQ(tok16, lexer->getToken());

    std::stringstream ss17;
    ss17 << "or";
    lexer->open(ss17);
    Token tok17("or", Type::orop, 1, 1);
    ASSERT_EQ(tok17, lexer->getToken());
}

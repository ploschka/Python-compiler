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


bool is_equal(std::string input_str, std::vector<Token> true_output_tokens) {
    std::stringstream ss;
    ss << input_str;
    lexer->open(ss);
    for (Token true_token: true_output_tokens) {
        Token curr_token = lexer->getToken(); 
        if (true_token != curr_token) {
            return false;
        }
    }
    return true;
}

bool is_equal(std::string input_str, Token true_output_token) {
    std::vector<Token> v = {true_output_token};
    return is_equal(input_str, v);
}


TEST(LexerTest, IdLettersOnly)
{
    ASSERT_TRUE(is_equal(
        "folga",
        Token("folga", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "abba",
        Token("abba", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "aa",
        Token("aa", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "a",
        Token("a", Type::id, 1, 1)
    ));
}

TEST(LexerTest, IdLettersAndNumbers)
{
    ASSERT_TRUE(is_equal(
        "folga123",
        Token("folga123", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "folg123a",
        Token("folg123a", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "f123olga",
        Token("f123olga", Type::id, 1, 1)  
    ));

    ASSERT_TRUE(is_equal(
        "fol1ga",
        Token("fol1ga", Type::id, 1, 1)
    ));   
}

TEST(LexerTest, IdUnderscore)
{
    ASSERT_TRUE(is_equal(
        "folga_",
        Token("folga_", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "folga__",
        Token("folga__", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "fol__ga",
        Token("fol__ga", Type::id, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "__folga",
        Token("__folga", Type::id, 1, 1)
    ));   
}

TEST(LexerTest, NumberInt)
{
    ASSERT_TRUE(is_equal(
        "221",
        Token("221", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "2281",
        Token("2281", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "0",
        Token("0", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "1234567034567890123456789876543234567",
        Token("1234567034567890123456789876543234567", Type::number, 1, 1)
    ));
}

TEST(LexerTest, NumberFloat)
{
    ASSERT_TRUE(is_equal(
        "22.1",
        Token("22.1", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "2.281",
        Token("2.281", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "0.0",
        Token("0.0", Type::number, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "0.1234567034567890123456789876543234567",
        Token("0.1234567034567890123456789876543234567", Type::number, 1, 1)
    ));   
}

TEST(LexerTest, Keyword)
{
    ASSERT_TRUE(is_equal(
        "def",
        Token("def", Type::defkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "if",
        Token("if", Type::ifkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "elif",
        Token("elif", Type::elifkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "else",
        Token("else", Type::elsekw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "for",
        Token("for", Type::forkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "while",
        Token("while", Type::whilekw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "class",
        Token("class", Type::classkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "yield",
        Token("yield", Type::yieldkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "return",
        Token("return", Type::returnkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "continue",
        Token("continue", Type::continuekw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "pass",
        Token("pass", Type::passkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "break",
        Token("break", Type::breakkw, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "not",
        Token("not", Type::notop, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "in",
        Token("in", Type::in, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "is",
        Token("is", Type::is, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "and",
        Token("and", Type::andop, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "or",
        Token("or", Type::orop, 1, 1)
    ));
}

TEST(LexerTest, Spaces)
{
    ASSERT_TRUE(is_equal(
        "a b c d",
        {
            Token("a", Type::id, 1, 1),
            Token("b", Type::id, 1, 3),
            Token("c", Type::id, 1, 5),
            Token("d", Type::id, 1, 7),
        }
    ));

    ASSERT_TRUE(is_equal(
        "a  b  c  d",
        {
            Token("a", Type::id, 1, 1),
            Token("b", Type::id, 1, 4),
            Token("c", Type::id, 1, 7),
            Token("d", Type::id, 1, 10),
        }
    ));

    ASSERT_TRUE(is_equal(
        "a b   c    d              e",
        {
            Token("a", Type::id, 1, 1),
            Token("b", Type::id, 1, 3),
            Token("c", Type::id, 1, 7),
            Token("d", Type::id, 1, 12),
            Token("e", Type::id, 1, 27),
        }
    ));
}


TEST(LexerTest, Strings)
{
    ASSERT_TRUE(is_equal(
        "\"\"",
        Token("", Type::string, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "\"goatse\"",
        Token("goatse", Type::string, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "\"нюхай бебру\"",
        Token("нюхай бебру", Type::string, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "\";DROP TABLE users;\"",
        Token(";DROP TABLE users;", Type::string, 1, 1)
    ));

    ASSERT_TRUE(is_equal(
        "\"\\n\"",
        Token("\\n", Type::string, 1, 1)
    ));
}


TEST(LexerTest, Newlines)
{
    ASSERT_TRUE(is_equal(
R"(a
)",
        {
            Token("a", Type::id, 1, 1),
            Token("", Type::newline, 1, 2),
        }
    ));

        ASSERT_TRUE(is_equal(
R"(a
b
c
)",
        {
            Token("a", Type::id, 1, 1),
            Token("", Type::newline, 1, 2),
            Token("b", Type::id, 2, 1),
            Token("", Type::newline, 2, 2),
            Token("c", Type::id, 3, 1),
        }
    ));

    ASSERT_TRUE(is_equal(
R"(a



b
)",
        {
            Token("a", Type::id, 1, 1),
            Token("", Type::newline, 1, 2),
            Token("b", Type::id, 5, 1),
        }
    ));
}


TEST(LexerTest, NewlinesIndentsDedents)
{
    ASSERT_TRUE(is_equal(
        "def f():\n\tbruh\n",
        {
            Token("def", Type::defkw, 1, 1),
            Token("f", Type::id, 1, 5),
            Token("", Type::lpr, 1, 6),
            Token("", Type::rpr, 1, 7),
            Token("", Type::colon, 1, 8),
            Token("", Type::newline, 1, 9),
            Token("", Type::indent, 2, 1),
            Token("bruh", Type::id, 2, 2),
            Token("", Type::newline, 2, 6),
            Token("", Type::dedent, 3, 1),
        }
    ));
}


TEST(LexerTest, End)
{
    ASSERT_TRUE(is_equal(
        "a",
        {
            Token("a", Type::id, 1, 1),
            Token("", Type::newline, 1, 2),
            Token("", Type::eof, 1, 3),
        }
    ));

    ASSERT_TRUE(is_equal(
        "",
        Token("", Type::eof, 1, 3)
    ));
    
    ASSERT_TRUE(is_equal(
        "def f():\n\tbruh",
        {
            Token("def", Type::defkw, 1, 1),
            Token("f", Type::id, 1, 5),
            Token("", Type::lpr, 1, 6),
            Token("", Type::rpr, 1, 7),
            Token("", Type::colon, 1, 8),
            Token("", Type::newline, 1, 9),
            Token("", Type::indent, 2, 1),
            Token("bruh", Type::id, 2, 2),
            Token("", Type::newline, 2, 6),
            Token("", Type::dedent, 3, 1),
            Token("", Type::eof, 3, 2),
        }
    ));
}
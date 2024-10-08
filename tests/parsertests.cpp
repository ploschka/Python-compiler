#include <gtest/gtest.h>

#include <pycom/parser/Parser.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/lexer/Lexer.hpp>

#include "NodeToVectorVisitor.h"
#include "LexerMock.hpp"

std::pair<std::vector<std::string>, std::vector<std::string>> convert(
    std::vector<Token> test_tokens, ProgramNode *target_root)
{
    /*
     * Тестовый набор токенов test_tokens подаётся в парсер
     * Парсер выдаёт синтаксическое дерево
     * Дерево, выданное парсером и контрольное дерево target_root преобразуются в векторы строк и возвращаются
     */
    auto lexer = new LexerMock(test_tokens);
    auto parser = new Parser;
    parser->setLexer(lexer);
    auto test_root = parser->getAST();

    auto test_visitor = new NodeToVectorVisitor();
    auto target_visitor = new NodeToVectorVisitor();
    test_root->accept(test_visitor);
    target_root->accept(target_visitor);
    std::vector<std::string> test_nodes = test_visitor->nodes;
    std::vector<std::string> target_nodes = target_visitor->nodes;

    return {
        test_nodes,
        target_nodes};
}

/*
a = 3
*/
TEST(ParserTest, ExpressionExpressionAssignment)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token("", Type::assign),
        Token("3", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new AssignmentNode(
             new Leaf(Token("a", Type::id)),
             new Leaf(Token("3", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a + 10
*/
TEST(ParserTest, ExpressionPlus)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token("", Type::plus),
        Token("10", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new Leaf(Token("a", Type::id)),
             new Leaf(Token("", Type::plus)),
             new Leaf(Token("10", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
1 + 2 + 3 + g
*/
TEST(ParserTest, ExpressionManyPlus)
{
    std::vector<Token> tokens = {
        Token("1", Type::number),
        Token("", Type::plus),
        Token("2", Type::number),
        Token("", Type::plus),
        Token("3", Type::number),
        Token("", Type::plus),
        Token("g", Type::id),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new BinaryNode(
                     new Leaf(Token("1", Type::number)),
                     new Leaf(Token("", Type::plus)),
                     new Leaf(Token("2", Type::number))),
                 new Leaf(Token("", Type::plus)),
                 new Leaf(Token("3", Type::number))),
             new Leaf(Token("", Type::plus)),
             new Leaf(Token("g", Type::id))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
1 + 2 - 3 + 4 - 5
*/
TEST(ParserTest, ExpressionPlusMinus)
{
    std::vector<Token> tokens = {
        Token("1", Type::number),
        Token("", Type::plus),
        Token("2", Type::number),
        Token("", Type::minus),
        Token("3", Type::number),
        Token("", Type::plus),
        Token("4", Type::number),
        Token("", Type::minus),
        Token("5", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new BinaryNode(
                     new BinaryNode(
                         new Leaf(Token("1", Type::number)),
                         new Leaf(Token("", Type::plus)),
                         new Leaf(Token("2", Type::number))),
                     new Leaf(Token("", Type::minus)),
                     new Leaf(Token("3", Type::number))),
                 new Leaf(Token("", Type::plus)),
                 new Leaf(Token("4", Type::number))),
             new Leaf(Token("", Type::minus)),
             new Leaf(Token("5", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a * b
*/
TEST(ParserTest, ExpressionMul)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token("", Type::star),
        Token("b", Type::id),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new Leaf(Token("a", Type::id)),
             new Leaf(Token("", Type::star)),
             new Leaf(Token("b", Type::id))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a * b / 100
*/
TEST(ParserTest, ExpressionMulWithDiv)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token("", Type::star),
        Token("b", Type::id),
        Token("", Type::div),
        Token("100", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new Leaf(Token("a", Type::id)),
                 new Leaf(Token("", Type::star)),
                 new Leaf(Token("b", Type::id))),
             new Leaf(Token("", Type::div)),
             new Leaf(Token("100", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a and b
*/
TEST(ParserTest, ExpressionAnd)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token("", Type::andop),
        Token("b", Type::id),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new Leaf(Token("a", Type::id)),
             new Leaf(Token("", Type::andop)),
             new Leaf(Token("b", Type::id))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
True or False
*/
TEST(ParserTest, ExpressionOr)
{
    std::vector<Token> tokens = {
        Token("True", Type::id),
        Token("", Type::orop),
        Token("False", Type::id),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new Leaf(Token("True", Type::id)),
             new Leaf(Token("", Type::orop)),
             new Leaf(Token("False", Type::id))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
not 1 + 2
*/
TEST(ParserTest, ExpressionNot)
{
    std::vector<Token> tokens = {
        Token("", Type::notop),
        Token("1", Type::number),
        Token("", Type::plus),
        Token("2", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new UnaryNode(
             new Leaf(Token("", Type::notop)),
             new BinaryNode(
                 new Leaf(Token("1", Type::number)),
                 new Leaf(Token("", Type::plus)),
                 new Leaf(Token("2", Type::number)))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
A and B or C or not D and F
*/
TEST(ParserTest, ExpressionLogicPriority)
{
    std::vector<Token> tokens = {
        Token("A", Type::id),
        Token("", Type::andop),
        Token("B", Type::id),
        Token("", Type::orop),
        Token("C", Type::id),
        Token("", Type::orop),
        Token("", Type::notop),
        Token("D", Type::id),
        Token("", Type::andop),
        Token("F", Type::id),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new BinaryNode(
                     new Leaf(Token("A", Type::id)),
                     new Leaf(Token("", Type::andop)),
                     new Leaf(Token("B", Type::id))),
                 new Leaf(Token("", Type::orop)),
                 new Leaf(Token("C", Type::id))),
             new Leaf(Token("", Type::orop)),
             new BinaryNode(
                 new UnaryNode(
                     new Leaf(Token("", Type::notop)),
                     new Leaf(Token("D", Type::id))),
                 new Leaf(Token("", Type::andop)),
                 new Leaf(Token("F", Type::id)))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
1 < x == 10 >= 100
*/
TEST(ParserTest, ExpressionComparisonEqualPriority)
{
    std::vector<Token> tokens = {
        Token("1", Type::number),
        Token("", Type::less),
        Token("x", Type::id),
        Token("", Type::equal),
        Token("10", Type::number),
        Token("", Type::grequal),
        Token("100", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new BinaryNode(
                     new Leaf(Token("1", Type::number)),
                     new Leaf(Token("", Type::less)),
                     new Leaf(Token("x", Type::id))),
                 new Leaf(Token("", Type::equal)),
                 new Leaf(Token("10", Type::number))),
             new Leaf(Token("", Type::grequal)),
             new Leaf(Token("100", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
-10
*/
TEST(ParserTest, ExpressionUnaryOp)
{
    std::vector<Token> tokens = {
        Token("", Type::minus),
        Token("10", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new UnaryNode(
             new Leaf(Token("", Type::minus)),
             new Leaf(Token("10", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
-10 / 0 + +x - -5 * 10
*/
TEST(ParserTest, ExpressionArithmOpPriority)
{
    std::vector<Token> tokens = {
        Token("-", Type::minus),
        Token("10", Type::number),
        Token("/", Type::div),
        Token("0", Type::number),
        Token("+", Type::plus),
        Token("+", Type::plus),
        Token("x", Type::id),
        Token("-", Type::minus),
        Token("-", Type::minus),
        Token("5", Type::number),
        Token("*", Type::star),
        Token("10", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new BinaryNode(
                     new UnaryNode(
                         new Leaf(Token("", Type::minus)),
                         new Leaf(Token("10", Type::number))),
                     new Leaf(Token("", Type::div)),
                     new Leaf(Token("0", Type::number))),
                 new Leaf(Token("", Type::plus)),
                 new UnaryNode(
                     new Leaf(Token("", Type::plus)),
                     new Leaf(Token("x", Type::id)))),
             new Leaf(Token("", Type::minus)),
             new BinaryNode(
                 new UnaryNode(
                     new Leaf(Token("", Type::minus)),
                     new Leaf(Token("5", Type::number))),
                 new Leaf(Token("", Type::star)),
                 new Leaf(Token("10", Type::number)))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
if True:
    break
*/
TEST(ParserTest, If)
{
    std::vector<Token> tokens = {
        Token("if", Type::ifkw),
        Token("True", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new IfNode(
             new Leaf(Token("True", Type::id)),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
if True:
    break
else:
    break
*/
TEST(ParserTest, IfIfElse)
{
    std::vector<Token> tokens = {
        Token("if", Type::ifkw),
        Token("True", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("else", Type::elsekw),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    auto if_node = new IfNode(
        new Leaf(Token("True", Type::id)),
        new BlockNode({new Leaf(Token("", Type::breakkw))}));
    auto else_node = new ElseNode(
        new BlockNode({new Leaf(Token("", Type::breakkw))}));
    if_node->next_else = else_node;
    ProgramNode *target_root = new ProgramNode({if_node,
                                                new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
if True:
    break
elif True:
    break
else:
    break
*/
TEST(ParserTest, IfElifElse)
{
    std::vector<Token> tokens = {
        Token("if", Type::ifkw),
        Token("True", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("elif", Type::elifkw),
        Token("True", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("else", Type::elsekw),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};

    auto if_node = new IfNode(
        new Leaf(Token("True", Type::id)),
        new BlockNode({new Leaf(Token("", Type::breakkw))}));
    auto elif_node = new ElifNode(
        new Leaf(Token("True", Type::id)),
        new BlockNode({new Leaf(Token("", Type::breakkw))}));
    auto else_node = new ElseNode(
        new BlockNode({new Leaf(Token("", Type::breakkw))}));
    if_node->next_elif = elif_node;
    elif_node->next_else = else_node;
    ProgramNode *target_root = new ProgramNode(
        {if_node,
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
while True:
    break
*/
TEST(ParserTest, While)
{
    std::vector<Token> tokens = {
        Token("while", Type::whilekw),
        Token("True", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new WhileNode(
             new Leaf(Token("True", Type::id)),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
for i in range(100):
    break
*/
TEST(ParserTest, For)
{
    std::vector<Token> tokens = {
        Token("for", Type::forkw),
        Token("i", Type::id),
        Token("in", Type::in),
        Token("range", Type::id),
        Token("(", Type::lpr),
        Token("100", Type::number),
        Token(")", Type::rpr),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new ForNode(
             new Leaf(Token("i", Type::id)),
             new CallNode(
                 Token("range", Type::id),
                 new ActualParamsNode({new Leaf(Token("100", Type::number))})),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
def func() -> None:
    break
*/
TEST(ParserTest, FunctionDefinitionNoParams)
{
    std::vector<Token> tokens = {
        Token("def", Type::defkw),
        Token("func", Type::id),
        Token("(", Type::lpr),
        Token(")", Type::rpr),
        Token("->", Type::arrow),
        Token("None", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new FunctionNode(
             new Leaf(Token("func", Type::id)),
             new TypeNode(Token("None", Type::id)),
             new FormalParamsNode({}, {}),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
def func(a: int, b: int) -> int:
    break
*/
TEST(ParserTest, FunctionDefinitionWithParams)
{
    std::vector<Token> tokens = {
        Token("def", Type::defkw),
        Token("func", Type::id),
        Token("(", Type::lpr),
        Token("a", Type::id),
        Token(":", Type::colon),
        Token("int", Type::id),
        Token(",", Type::comma),
        Token("b", Type::id),
        Token(":", Type::colon),
        Token("int", Type::id),
        Token(")", Type::rpr),
        Token("->", Type::arrow),
        Token("int", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new FunctionNode(
             new Leaf(Token("func", Type::id)),
             new TypeNode(Token("int", Type::id)),
             new FormalParamsNode(
                 {new Leaf(Token("a", Type::id)),
                  new Leaf(Token("b", Type::id))},
                 {new TypeNode(Token("int", Type::id)),
                  new TypeNode(Token("int", Type::id))}),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
print()
*/
TEST(ParserTest, FunctionCallNoParams)
{
    std::vector<Token> tokens = {
        Token("print", Type::id),
        Token("(", Type::lpr),
        Token(")", Type::rpr),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new CallNode(
             Token("print", Type::id),
             new ActualParamsNode({})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
print(1, 2, 3)
*/
TEST(ParserTest, FunctionCallWithParams)
{
    std::vector<Token> tokens = {
        Token("print", Type::id),
        Token("(", Type::lpr),
        Token("1", Type::number),
        Token(",", Type::comma),
        Token("2", Type::number),
        Token(",", Type::comma),
        Token("3", Type::number),
        Token(")", Type::rpr),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new CallNode(
             Token("print", Type::id),
             new ActualParamsNode({new Leaf(Token("1", Type::number)),
                                   new Leaf(Token("2", Type::number)),
                                   new Leaf(Token("3", Type::number))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
def func() -> list[int]:
    return
*/
TEST(ParserTest, ReturnNothing)
{
    std::vector<Token> tokens = {
        Token("def", Type::defkw),
        Token("func", Type::id),
        Token("(", Type::lpr),
        Token(")", Type::rpr),
        Token("->", Type::arrow),
        Token("list", Type::id),
        Token("[", Type::lsbr),
        Token("int", Type::id),
        Token("]", Type::rsbr),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("return", Type::returnkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new FunctionNode(
             new Leaf(Token("func", Type::id)),
             new TypeNode(Token("int", Type::id), true),
             new FormalParamsNode({}, {}),
             new BlockNode({new ReturnNode(
                 new Leaf(Token("None", Type::id)))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
def func() -> None:
    return 1 + 2
*/
TEST(ParserTest, ReturnSomething)
{
    std::vector<Token> tokens = {
        Token("def", Type::defkw),
        Token("func", Type::id),
        Token("(", Type::lpr),
        Token(")", Type::rpr),
        Token("->", Type::arrow),
        Token("None", Type::id),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("return", Type::returnkw),
        Token("1", Type::number),
        Token("+", Type::plus),
        Token("2", Type::number),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new FunctionNode(
             new Leaf(Token("func", Type::id)),
             new TypeNode(Token("None", Type::id)),
             new FormalParamsNode({}, {}),
             new BlockNode({new ReturnNode(
                 new BinaryNode(
                     new Leaf(Token("1", Type::number)),
                     new Leaf(Token("", Type::plus)),
                     new Leaf(Token("2", Type::number))))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
[1 + 2, a, print()]
*/
TEST(ParserTest, List)
{
    std::vector<Token> tokens = {
        Token("[", Type::lsbr),
        Token("1", Type::number),
        Token("+", Type::plus),
        Token("2", Type::number),
        Token(",", Type::comma),
        Token("a", Type::id),
        Token(",", Type::comma),
        Token("print", Type::id),
        Token("(", Type::lpr),
        Token(")", Type::rpr),
        Token("]", Type::rsbr),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new ListNode({
             new BinaryNode(
                 new Leaf(Token("1", Type::number)),
                 new Leaf(Token("+", Type::plus)),
                 new Leaf(Token("2", Type::number))),
             new Leaf(Token("a", Type::id)),
             new CallNode(
                 Token("print", Type::id),
                 new ActualParamsNode({})),
         }),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
[]
*/
TEST(ParserTest, EmptyList)
{
    std::vector<Token> tokens = {
        Token("[", Type::lsbr),
        Token("]", Type::rsbr),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new ListNode({}),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
(1 + 2) * 3
*/
TEST(ParserTest, ExpressionWithParenthesis)
{
    std::vector<Token> tokens = {
        Token("(", Type::lpr),
        Token("1", Type::number),
        Token("+", Type::plus),
        Token("2", Type::number),
        Token(")", Type::rpr),
        Token("*", Type::star),
        Token("3", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new BinaryNode(
             new BinaryNode(
                 new Leaf(Token("1", Type::number)),
                 new Leaf(Token("+", Type::plus)),
                 new Leaf(Token("2", Type::number))),
             new Leaf(Token("*", Type::star)),
             new Leaf(Token("3", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
if (True):
    break
*/
TEST(ParserTest, ConditionWithParenthesis)
{
    std::vector<Token> tokens = {
        Token("if", Type::ifkw),
        Token("(", Type::lpr),
        Token("True", Type::id),
        Token(")", Type::rpr),
        Token(":", Type::colon),
        Token("", Type::newline),
        Token("", Type::indent),
        Token("break", Type::breakkw),
        Token("", Type::newline),
        Token("", Type::dedent),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new IfNode(
             new Leaf(Token("True", Type::id)),
             new BlockNode({new Leaf(Token("", Type::breakkw))})),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a: int = 5
*/
TEST(ParserTest, AssignWithType)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token(":", Type::colon),
        Token("int", Type::id),
        Token("=", Type::assign),
        Token("5", Type::number),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new AssignmentNode(
             new Leaf(Token("a", Type::id)),
             new TypeNode(Token("int", Type::id)),
             new Leaf(Token("5", Type::number))),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

/*
a: list[int] = [1, 2, 3]
*/
TEST(ParserTest, AssignWithTypeList)
{
    std::vector<Token> tokens = {
        Token("a", Type::id),
        Token(":", Type::colon),
        Token("list", Type::id),
        Token("[", Type::lsbr),
        Token("int", Type::id),
        Token("]", Type::rsbr),
        Token("=", Type::assign),
        Token("[", Type::lsbr),
        Token("1", Type::number),
        Token(",", Type::comma),
        Token("2", Type::number),
        Token(",", Type::comma),
        Token("3", Type::number),
        Token("]", Type::rsbr),
        Token("", Type::newline),
        Token("", Type::eof)};
    ProgramNode *target_root = new ProgramNode(
        {new AssignmentNode(
             new Leaf(Token("a", Type::id)),
             new TypeNode(Token("int", Type::id), true),
             new ListNode({
                 new Leaf(Token("1", Type::number)),
                 new Leaf(Token("2", Type::number)),
                 new Leaf(Token("3", Type::number)),
             })),
         new Leaf(Token("", Type::eof))});
    auto nodes = convert(tokens, target_root);
    auto test_nodes = nodes.first;
    auto target_nodes = nodes.second;
    ASSERT_EQ(test_nodes, target_nodes);
}

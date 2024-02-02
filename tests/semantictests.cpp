#include <pycom/semanalyzer/SemanticAnalyzer.hpp>
#include <gtest/gtest.h>
#include <pycom/parser/Parser.hpp>
#include <pycom/token/Token.hpp>
#include "ThrowErrorManager.hpp"

static std::unique_ptr<SemanticAnalyzer> semantic = std::make_unique<SemanticAnalyzer>();

/*
Неизвестная переменная
b: int = 1 + a
*/
TEST(SemanticTest, UndefinedVariable)
{
    semantic->setEM(new ThrowErrorManager());
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
            new Leaf(Token("b", Type::id)),
            new TypeNode(Token("int", Type::id)),
            new BinaryNode(
                new Leaf(Token("1", Type::number)),
                new Leaf(Token("+", Type::plus)),
                new Leaf(Token("a", Type::id))))}));
    ASSERT_ANY_THROW(semantic->checkSemantics(ast));
}

/*
Известная переменная
a: int = 2
b: int = 1 + a
*/
TEST(SemanticTest, DefinedVariable)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
                             new Leaf(Token("a", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new Leaf(Token("2", Type::number))),
                         new AssignmentNode(
                             new Leaf(Token("b", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new BinaryNode(
                                 new Leaf(Token("1", Type::number)),
                                 new Leaf(Token("+", Type::plus)),
                                 new Leaf(Token("a", Type::id))))}));
    ASSERT_NO_THROW(semantic->checkSemantics(ast));
}

/*
Несовместимые типы
a: int = 10
b: str = "deez nutz"
c: int = a + b
*/
TEST(SemanticTest, TypesNotMatch)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
                             new Leaf(Token("a", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new Leaf(Token("10", Type::number))),
                         new AssignmentNode(
                             new Leaf(Token("b", Type::id)),
                             new TypeNode(Token("str", Type::id)),
                             new Leaf(Token("deez nutz", Type::string))),
                         new AssignmentNode(
                             new Leaf(Token("c", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new BinaryNode(
                                 new Leaf(Token("a", Type::id)),
                                 new Leaf(Token("+", Type::plus)),
                                 new Leaf(Token("b", Type::id))))}));
    ASSERT_ANY_THROW(semantic->checkSemantics(ast));
}

/*
Совместимые типы
a: str = "ab"
b: str = "deez nutz"
c: str = a + b
*/
TEST(SemanticTest, TypesMatch)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
                             new Leaf(Token("a", Type::id)),
                             new TypeNode(Token("str", Type::id)),
                             new Leaf(Token("ab", Type::string))),
                         new AssignmentNode(
                             new Leaf(Token("b", Type::id)),
                             new TypeNode(Token("str", Type::id)),
                             new Leaf(Token("deez nutz", Type::string))),
                         new AssignmentNode(
                             new Leaf(Token("c", Type::id)),
                             new TypeNode(Token("str", Type::id)),
                             new BinaryNode(
                                 new Leaf(Token("a", Type::id)),
                                 new Leaf(Token("+", Type::plus)),
                                 new Leaf(Token("b", Type::id))))}));
    ASSERT_NO_THROW(semantic->checkSemantics(ast));
}

/*
Вызов несуществующей функции
fuck()
*/
TEST(SemanticTest, UndefinedFunction)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new CallNode(
            Token("fuck", Type::id),
            new ActualParamsNode({}))}));
    ASSERT_ANY_THROW(semantic->checkSemantics(ast));
}

/*
Вызов существующей функции
def fuck() -> str:
    return "fuck"
fuck()
*/
TEST(SemanticTest, DefinedFunction)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new FunctionNode(
                             new Leaf(Token("fuck", Type::id)),
                             new TypeNode(Token("str", Type::id)),
                             new FormalParamsNode({}, {}),
                             new BlockNode({new ReturnNode(
                                 new Leaf(Token("fuck", Type::string)))})),
                         new CallNode(
                             Token("fuck", Type::id),
                             new ActualParamsNode({}))}));
    ASSERT_NO_THROW(semantic->checkSemantics(ast));
}

// /*
// Возвращаемое значение не то
// def f() -> None:
//     return 1
// */
// TEST(SemanticTest, ReturnWrongType)
// {
//     semantic->reset();
//     AST *ast = new AST(
//         new ProgramNode({new FunctionNode(
//             new Leaf(Token("f", Type::id)),
//             new TypeNode(Token("None", Type::id)),
//             new FormalParamsNode({}, {}),
//             new BlockNode({new ReturnNode(
//                 new Leaf(Token("1", Type::number)))}))}));
//     ASSERT_ANY_THROW(semantic->checkSemantics(ast));
// }

// /*
// Возвращаемое значение то
// def f() -> int:
//     return 1
// */
// TEST(SemanticTest, ReturnRightType)
// {
//     semantic->reset();
//     AST *ast = new AST(
//         new ProgramNode({new FunctionNode(
//             new Leaf(Token("f", Type::id)),
//             new TypeNode(Token("int", Type::id)),
//             new FormalParamsNode({}, {}),
//             new BlockNode({new ReturnNode(
//                 new Leaf(Token("1", Type::number)))}))}));
//     ASSERT_NO_THROW(semantic->checkSemantics(ast));
// }

/*
Неправильный тип аргументов при вызове функции
def f(a: int, b: str) -> int:
    return 0
f("1", 2)
*/
TEST(SemanticTest, CallArgumentsWrongType)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new FunctionNode(
                             new Leaf(Token("f", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new FormalParamsNode(
                                 {new Leaf(Token("a", Type::id)),
                                  new Leaf(Token("b", Type::id))},
                                 {new TypeNode(Token("int", Type::id)),
                                  new TypeNode(Token("str", Type::id))}),
                             new BlockNode({new ReturnNode(
                                 new Leaf(Token("0", Type::number)))})),
                         new CallNode(
                             Token("f", Type::id),
                             new ActualParamsNode({new Leaf(Token("1", Type::string)),
                                                   new Leaf(Token("2", Type::number))}))}));
    ASSERT_ANY_THROW(semantic->checkSemantics(ast));
}

/*
Правильный тип аргументов при вызове функции
def f(a: int, b: str) -> int:
    return 0
f(1, "2")
*/
TEST(SemanticTest, CallArgumentsRightType)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new FunctionNode(
                             new Leaf(Token("f", Type::id)),
                             new TypeNode(Token("int", Type::id)),
                             new FormalParamsNode(
                                 {new Leaf(Token("a", Type::id)),
                                  new Leaf(Token("b", Type::id))},
                                 {new TypeNode(Token("int", Type::id)),
                                  new TypeNode(Token("str", Type::id))}),
                             new BlockNode({new ReturnNode(
                                 new Leaf(Token("0", Type::number)))})),
                         new CallNode(
                             Token("f", Type::id),
                             new ActualParamsNode({new Leaf(Token("1", Type::number)),
                                                   new Leaf(Token("2", Type::string))}))}));
    ASSERT_NO_THROW(semantic->checkSemantics(ast));
}

/*
Не тот тип в списке
a: list[int] = ["hello", 5]
*/
TEST(SemanticTest, ListValuesWrongType)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
            new Leaf(Token("a", Type::id)),
            new TypeNode(Token("int", Type::id), true),
            new ListNode({
                new Leaf(Token("hello", Type::string)),
                new Leaf(Token("5", Type::number)),
            }))}));
    ASSERT_ANY_THROW(semantic->checkSemantics(ast));
}

/*
Тот тип в списке
a: list[int] = [69, 5]
*/
TEST(SemanticTest, ListValuesRightType)
{
    semantic->reset();
    AST *ast = new AST(
        new ProgramNode({new AssignmentNode(
            new Leaf(Token("a", Type::id)),
            new TypeNode(Token("int", Type::id), true),
            new ListNode({
                new Leaf(Token("69", Type::number)),
                new Leaf(Token("5", Type::number)),
            }))}));
    ASSERT_NO_THROW(semantic->checkSemantics(ast));
}

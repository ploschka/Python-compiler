#pragma once
#include <pycom/interface/ParserInterface.hpp>
#include <vector>
#include "pycom/AST/ASTNode.hpp"
#include "pycom/AST/AST.hpp"

class Parser : public ParserInterface
{
private:
    LexerInterface* lexer;
    Token token;  // Текущий токен кода
    std::vector<Token> future_tokens; // Сюда будут складываться токены при просмотре "наперёд" методом forward

    bool is_token_in_firsts(std::string grammar_node);
    bool token_matches_any(std::vector<Type> types);
    void error(std::string message);
    void next_token();
    Token forward(int k);
    Token get_token();
    Token check_get_next(Type type);

    ProgramNode * file();
    void statements(BlockNode *parent_block);
    void statement(BlockNode *parent_block);
    void simple_stmts(BlockNode *parent_block);
    void simple_stmt(BlockNode *parent_block);
    AssignmentNode * assignment();
    ReturnNode * return_stmt();
    ExpressionNode * expression();
    ExpressionNode * disjunction();
    ExpressionNode * conjunction();
    ExpressionNode * inversion();
    ExpressionNode * sum();
    ExpressionNode * term();
    ExpressionNode * factor();
    ExpressionNode * primary();
    ActualParamsNode * func_call();
    Leaf * atom();
    ActualParamsNode * arguments();
    void compound_stmt(BlockNode *parent_block);
    FormalParamsNode * params();
    FunctionNode * function_def();
    IfNode * if_stmt();
    ElifNode * elif_stmt();
    ElseNode * else_block();
    WhileNode * while_stmt();
    ForNode * for_stmt();
    BlockNode * block();

public:
    void setLexer(LexerInterface*);
    AST* getAST();
    Parser();
};

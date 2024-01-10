#pragma once

#include <pycom/interface/ParserInterface.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <pycom/AST/AST.hpp>

#include <vector>

class Parser : public ParserInterface
{
private:
    LexerInterface *lexer;
    Token token;                      // Текущий токен кода
    std::vector<Token> future_tokens; // Сюда будут складываться токены при просмотре "наперёд" методом forward

    bool is_token_in_firsts(std::string _grammar_node);
    bool token_matches_any(std::vector<Type> _types);
    bool token_matches(Type _type);
    void error(std::string _message);
    void next_token();
    Token forward(size_t _k);
    Token get_token();
    Token check_get_next(Type _type);

    ProgramNode *file();
    void statements(BlockNode *_parent_block);
    void statement(BlockNode *_parent_block);
    void simple_stmts(BlockNode *_parent_block);
    void simple_stmt(BlockNode *_parent_block);
    AssignmentNode *assignment();
    ReturnNode *return_stmt();
    ExpressionNode *expression();
    ExpressionNode *disjunction();
    ExpressionNode *conjunction();
    ExpressionNode *inversion();
    ExpressionNode *comparison();
    ExpressionNode *sum();
    ExpressionNode *term();
    ExpressionNode *factor();
    ExpressionNode *primary();
    ListNode *list();
    std::vector<ExpressionNode *> list_values();
    ActualParamsNode *func_call();
    Leaf *atom();
    TypeNode *type_hint();
    ActualParamsNode *arguments();
    void compound_stmt(BlockNode *_parent_block);
    FormalParamsNode *params();
    FunctionNode *function_def();
    IfNode *if_stmt();
    ElifNode *elif_stmt();
    ElseNode *else_block();
    WhileNode *while_stmt();
    ForNode *for_stmt();
    BlockNode *block();


public:
    void setLexer(LexerInterface *);
    AST *getAST();
    Parser();
};

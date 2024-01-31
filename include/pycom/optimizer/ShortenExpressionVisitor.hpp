#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/token/Token.hpp>
#include <pycom/optimizer/CalculateVisitor.hpp>
#include <vector>
#include <map>

struct VariableTable
{
    std::map<std::string, ExpressionNode *> variables;
    std::map<std::string, bool> is_var_constant; // Была ли переменная сокращена до константы
};

class ShortenExpressionVisitor : public NodeVisitorInterface
{
    // Возвращаемые узлами значения:
    bool is_constant;              // Был ли узел сокращён до константы
    ExpressionNode *shortenedExpr; // Сокращённое значение

    CalculateVisitor *calculator = new CalculateVisitor();
    std::vector<VariableTable> scopes_stack;

    bool is_var_constant(std::string id);
    ExpressionNode *get_var(std::string id);
    void set_var(std::string id, ExpressionNode *node, bool is_constant);
    void add_scope();
    void remove_scope();

public:
    void visitLeaf(Leaf *_acceptor);
    void visitTypeNode(TypeNode *_acceptor);
    void visitFormalParamsNode(FormalParamsNode *_acceptor);
    void visitActualParamsNode(ActualParamsNode *_acceptor);
    void visitCallNode(CallNode *_acceptor);
    void visitBinaryNode(BinaryNode *_acceptor);
    void visitUnaryNode(UnaryNode *_acceptor);
    void visitAssignmentNode(AssignmentNode *_acceptor);
    void visitReturnNode(ReturnNode *_acceptor);
    void visitBlockNode(BlockNode *_acceptor);
    void visitProgramNode(ProgramNode *_acceptor);
    void visitFunctionNode(FunctionNode *_acceptor);
    void visitElseNode(ElseNode *_acceptor);
    void visitElifNode(ElifNode *_acceptor);
    void visitIfNode(IfNode *_acceptor);
    void visitWhileNode(WhileNode *_acceptor);
    void visitForNode(ForNode *_acceptor);
    void visitListNode(ListNode *_acceptor);
};
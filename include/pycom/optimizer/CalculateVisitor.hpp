#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <pycom/token/Token.hpp>

/**
 * Визитёр, вычисляющий значения некоторых узлов.
 * Например, для Бинарного узла 2 + 3 результат будет 5.
 */
class CalculateVisitor : public NodeVisitorInterface
{
private:
    bool has_type(ExpressionNode *node, std::string type);
    std::string calculate_binary_string(std::string left, std::string right, Type op);
    int calculate_binary_int(int left, int right, Type op);
    int calculate_unary_int(int value, Type op);

public:
    std::string return_result;
    Type return_type;

    void visitLeaf(Leaf *_acceptor);
    void visitBinaryNode(BinaryNode *_acceptor);
    void visitUnaryNode(UnaryNode *_acceptor);

    void visitTypeNode(TypeNode *_acceptor);
    void visitFormalParamsNode(FormalParamsNode *_acceptor);
    void visitActualParamsNode(ActualParamsNode *_acceptor);
    void visitCallNode(CallNode *_acceptor);
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

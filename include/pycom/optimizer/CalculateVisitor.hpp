#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <pycom/token/Token.hpp>
#include <vector>


/**
 * Визитёр, вычисляющий значения некоторых узлов.
 * Например, для Бинарного узла 2 + 3 результат будет 5.
 */

class CalculateVisitor: public NodeVisitorInterface {
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

    void visitTypeNode(TypeNode *) {};
    void visitFormalParamsNode(FormalParamsNode *) {};
    void visitActualParamsNode(ActualParamsNode *) {};
    void visitCallNode(CallNode *) {};
    void visitAssignmentNode(AssignmentNode *) {};
    void visitReturnNode(ReturnNode *) {};
    void visitBlockNode(BlockNode *) {};
    void visitProgramNode(ProgramNode *) {};
    void visitFunctionNode(FunctionNode *) {};
    void visitElseNode(ElseNode *) {};
    void visitElifNode(ElifNode *) {};
    void visitIfNode(IfNode *) {};
    void visitWhileNode(WhileNode *) {};
    void visitForNode(ForNode *) {};
    void visitListNode(ListNode *) {};
};
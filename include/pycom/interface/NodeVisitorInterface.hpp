#pragma once
#include <pycom/AST/ASTNodePublic.hpp>

class NodeVisitorInterface
{
public:
    virtual ~NodeVisitorInterface() = default;

    virtual void visitLeaf(Leaf* acceptor) const = 0;
    virtual void visitFormalParamsNode(FormalParamsNode* acceptor) const = 0;
    virtual void visitActualParamsNode(ActualParamsNode* acceptor) const = 0;
    virtual void visitVariableNode(VariableNode* acceptor) const = 0;
    virtual void visitCallNode(CallNode* acceptor) const = 0;
    virtual void visitBinaryNode(BinaryNode* acceptor) const = 0;
    virtual void visitUnaryNode(UnaryNode* acceptor) const = 0;
    virtual void visitAssignmentNode(AssignmentNode* acceptor) const = 0;
    virtual void visitReturnNode(ReturnNode* acceptor) const = 0;
    virtual void visitBlockNode(BlockNode* acceptor) const = 0;
    virtual void visitProgramNode(ProgramNode* acceptor) const = 0;
    virtual void visitFunctionNode(FunctionNode* acceptor) const = 0;
    virtual void visitElseNode(ElseNode* acceptor) const = 0;
    virtual void visitElifNode(ElifNode* acceptor) const = 0;
    virtual void visitIfNode(IfNode* acceptor) const = 0;
    virtual void visitWhileNode(WhileNode* acceptor) const = 0;
    virtual void visitForNode(ForNode* acceptor) const = 0;
};

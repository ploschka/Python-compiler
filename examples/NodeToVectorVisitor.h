#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <vector>
#include <string>

class NodeToVectorVisitor : public NodeVisitorInterface
{
public:
    std::vector<std::string> nodes;

    void visitLeaf(Leaf *_acceptor) override;
    void visitTypeNode(TypeNode *_acceptor) override;
    void visitFormalParamsNode(FormalParamsNode *_acceptor) override;
    void visitActualParamsNode(ActualParamsNode *_acceptor) override;
    void visitCallNode(CallNode *_acceptor) override;
    void visitBinaryNode(BinaryNode *_acceptor) override;
    void visitUnaryNode(UnaryNode *_acceptor) override;
    void visitAssignmentNode(AssignmentNode *_acceptor) override;
    void visitReturnNode(ReturnNode *_acceptor) override;
    void visitBlockNode(BlockNode *_acceptor) override;
    void visitProgramNode(ProgramNode *_acceptor) override;
    void visitFunctionNode(FunctionNode *_acceptor) override;
    void visitElseNode(ElseNode *_acceptor) override;
    void visitElifNode(ElifNode *_acceptor) override;
    void visitIfNode(IfNode *_acceptor) override;
    void visitWhileNode(WhileNode *_acceptor) override;
    void visitForNode(ForNode *_acceptor) override;
    void visitListNode(ListNode *_acceptor) override;
};

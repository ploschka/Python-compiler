#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/AST/ASTNode.hpp>

class PrintVisitor: public NodeVisitorInterface {
    int indent = 0;  // Количество отступов при выводе
    std::string indent_str = "   |";
    void print_line(std::string text);

public:
    void visitLeaf(Leaf* acceptor) override;
    void visitFormalParamsNode(FormalParamsNode* acceptor) override;
    void visitActualParamsNode(ActualParamsNode* acceptor) override;
    void visitVariableNode(VariableNode* acceptor) override;
    void visitCallNode(CallNode* acceptor) override;
    void visitBinaryNode(BinaryNode* acceptor) override;
    void visitUnaryNode(UnaryNode* acceptor) override;
    void visitAssignmentNode(AssignmentNode* acceptor) override;
    void visitReturnNode(ReturnNode* acceptor) override;
    void visitBlockNode(BlockNode* acceptor) override;
    void visitProgramNode(ProgramNode* acceptor) override;
    void visitFunctionNode(FunctionNode* acceptor) override;
    void visitElseNode(ElseNode* acceptor) override;
    void visitElifNode(ElifNode* acceptor) override;
    void visitIfNode(IfNode* acceptor) override;
    void visitWhileNode(WhileNode* acceptor) override;
    void visitForNode(ForNode* acceptor) override;
};


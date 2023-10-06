#pragma once
#include <pycom/interface/SemanticAnalyzerInterface.hpp>
#include <string>
#include <map>
#include <vector>


class SemanticAnalyzer: public NodeVisitorInterface {
private:
    SymbolTableStack tableStack;

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
    void visitFunctionNode(FunctionNode* acceptor) override;
    void visitElseNode(ElseNode* acceptor) override;
    void visitElifNode(ElifNode* acceptor) override;
    void visitIfNode(IfNode* acceptor) override;
    void visitWhileNode(WhileNode* acceptor) override;
    void visitForNode(ForNode* acceptor) override;
    void visitProgramNode(ProgramNode* acceptor);
};

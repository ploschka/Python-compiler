#pragma once
#include <pycom/AST/AST.hpp>
#include <pycom/AST/ASTNodePublic.hpp>
#include <pycom/semanalyzer/SymbolTableStack.hpp>


class SemanticAnalyzerInterface {
public:
    virtual ~SemanticAnalyzerInterface() = default;

    virtual void visitLeaf(Leaf* acceptor) = 0;
    virtual void visitFormalParamsNode(FormalParamsNode* acceptor) = 0;
    virtual void visitActualParamsNode(ActualParamsNode* acceptor) = 0;
    virtual void visitVariableNode(VariableNode* acceptor) = 0;
    virtual void visitCallNode(CallNode* acceptor) = 0;
    virtual void visitBinaryNode(BinaryNode* acceptor) = 0;
    virtual void visitUnaryNode(UnaryNode* acceptor) = 0;
    virtual void visitAssignmentNode(AssignmentNode* acceptor) = 0;
    virtual void visitReturnNode(ReturnNode* acceptor) = 0;
    virtual void visitBlockNode(BlockNode* acceptor) = 0;
    virtual void visitFunctionNode(FunctionNode* acceptor) = 0;
    virtual void visitElseNode(ElseNode* acceptor) = 0;
    virtual void visitElifNode(ElifNode* acceptor) = 0;
    virtual void visitIfNode(IfNode* acceptor) = 0;
    virtual void visitWhileNode(WhileNode* acceptor) = 0;
    virtual void visitForNode(ForNode* acceptor) = 0;
    virtual bool isSemanticallyCorrect(AST* tree) = 0;
};

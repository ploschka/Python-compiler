#pragma once
#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/token/Token.hpp>
#include <map>
#include <queue>
#include <stack>
#include <utility>
#include <ostream>

class CodeEmittingNodeVisitor : public NodeVisitorInterface
{
private:
    std::ostream& stream;
    void write(std::string s);
    void write(Token token);
    void write(Leaf* leaf);
public:
    CodeEmittingNodeVisitor(std::ostream& _stream);
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

#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/NodeVisitorInterface.hpp>

#include <map>
typedef std::map<std::string, llvm::Value *> symbtable_t;

class CodeEmittingNodeVisitor : public NodeVisitorInterface
{
private:
    llvm::IRBuilder<> *builder;
    llvm::Module *module;
    llvm::LLVMContext *context;
    symbtable_t namedValues;

public:
    void visitLeaf(Leaf *_acceptor);
    void visitFormalParamsNode(FormalParamsNode *_acceptor);
    void visitActualParamsNode(ActualParamsNode *_acceptor);
    void visitVariableNode(VariableNode *_acceptor);
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
    CodeEmittingNodeVisitor(llvm::IRBuilder<> *_builder, llvm::Module *_module, llvm::LLVMContext *_context);
};

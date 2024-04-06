#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/NodeVisitorInterface.hpp>
#include <pycom/interface/ErrorManagerInterface.hpp>
#include <pycom/interface/ErrorEmitterInterface.hpp>

#include <map>
#include <queue>
#include <stack>
#include <utility>
#include <tuple>

class GlobalNodeVisitor : public NodeVisitorInterface, public ErrorEmitterInterface
{
private:
    typedef std::map<std::string, llvm::Value *> symbtable_t;
    typedef std::tuple<llvm::Value *, llvm::Value *, llvm::BasicBlock *> phival_t;

    llvm::IRBuilder<> *builder;
    llvm::Module *module;
    llvm::LLVMContext *context;

    ErrorManagerInterface *em;

    void stdinit();
    void error(const std::string &_str);

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
    GlobalNodeVisitor(llvm::IRBuilder<> *_builder,
                            llvm::Module *_module,
                            llvm::LLVMContext *_context);

    void setEM(ErrorManagerInterface *_em);
};

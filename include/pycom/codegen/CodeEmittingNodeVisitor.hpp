#pragma once

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>

#include <pycom/interface/NodeVisitorInterface.hpp>

#include <map>
#include <queue>
#include <stack>
#include <utility>

typedef std::map<std::string, llvm::Value *> symbtable_t;
typedef std::tuple<llvm::Value *, llvm::Value *, llvm::BasicBlock *> phival_t;

class CodeEmittingNodeVisitor : public NodeVisitorInterface
{
private:
    llvm::IRBuilder<> *builder;
    llvm::Module *module;
    llvm::LLVMContext *context;

    symbtable_t namedValues; // TODO std::stack<std::unique_ptr<symtable_t>>
    std::queue<llvm::Value *> stored_values;
    std::vector<llvm::Value *> stored_array;
    llvm::BasicBlock *main_block;
    std::stack<llvm::BasicBlock *> break_stack;
    std::stack<llvm::BasicBlock *> continue_stack;
    std::stack<std::unique_ptr<std::vector<phival_t>>> phi_stack;

    llvm::Value *getLeafValue(Leaf *_leaf);
    void stdinit();

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

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

class CodeEmittingNodeVisitor : public NodeVisitorInterface, public ErrorEmitterInterface
{
private:
    typedef std::map<std::string, llvm::Value *> symbtable_t;
    typedef std::tuple<llvm::Value *, llvm::Value *, llvm::BasicBlock *> phival_t;

    llvm::IRBuilder<> *builder;
    llvm::Module *module;
    llvm::LLVMContext *context;

    std::stack<symbtable_t> namedValues;
    std::queue<llvm::Value *> stored_values;
    std::vector<llvm::Value *> stored_array;
    llvm::BasicBlock *main_block;
    llvm::BasicBlock *return_block = nullptr;
    llvm::Value *return_value = nullptr;
    bool block_ended = false;
    std::stack<llvm::BasicBlock *> break_stack;
    std::stack<llvm::BasicBlock *> continue_stack;
    std::stack<llvm::BasicBlock *> merge_stack;
    unsigned int lastpos;
    unsigned int lastrow;

    ErrorManagerInterface *em;

    enum class my_type
    {
        int_type,
        str_type,
        bool_type,
        void_type
    };
    static const std::unordered_map<std::string, my_type> typemap;

    std::tuple<CodeEmittingNodeVisitor::my_type, llvm::Type *> __str_to_type(const std::string &_str);
    std::tuple<CodeEmittingNodeVisitor::my_type, llvm::Type *, bool> str_to_type(const std::string &_str);
    llvm::Value *getLeafValue(Leaf *_leaf);
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
    CodeEmittingNodeVisitor(llvm::IRBuilder<> *_builder,
                            llvm::Module *_module,
                            llvm::LLVMContext *_context);

    void setEM(ErrorManagerInterface *_em);
};

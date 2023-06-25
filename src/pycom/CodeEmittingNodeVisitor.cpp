#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>

#include <pycom/codegen/CodeEmittingNodeVisitor.hpp>
#include <pycom/AST/ASTNode.hpp>

static inline llvm::Value *getLeafValue(symbtable_t &_table, llvm::IRBuilder<> *_builder, Leaf *_leaf)
{
    Type t = _leaf->token.getType();
    std::string val = _leaf->token.getValue();
    if (t == Type::number)
    {
        auto type = llvm::Type::getFloatTy(_builder->getContext());
        return llvm::ConstantFP::get(type, llvm::StringRef(val));
    }
    if (t == Type::id)
    {
        llvm::Value *v = _table[val];
        return _builder->CreateLoad(llvm::Type::getDoubleTy(_builder->getContext()), v, val.c_str());
    }
    return nullptr;
}

CodeEmittingNodeVisitor::CodeEmittingNodeVisitor(
    llvm::IRBuilder<> *_builder,
    llvm::Module *_module,
    llvm::LLVMContext *_context) : builder(_builder), module(_module), context(_context) {}

void CodeEmittingNodeVisitor::visitLeaf(Leaf *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitFormalParamsNode(FormalParamsNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitVariableNode(VariableNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitCallNode(CallNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitReturnNode(ReturnNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitBlockNode(BlockNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitProgramNode(ProgramNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitElseNode(ElseNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitElifNode(ElifNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitIfNode(IfNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitWhileNode(WhileNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitForNode(ForNode *_acceptor)
{
}

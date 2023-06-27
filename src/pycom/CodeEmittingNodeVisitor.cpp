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
        if (_table.find(val) == _table.end())
        {
            return nullptr;
        }
        return _table[val];
    }
    return nullptr;
}

CodeEmittingNodeVisitor::CodeEmittingNodeVisitor(
    llvm::IRBuilder<> *_builder,
    llvm::Module *_module,
    llvm::LLVMContext *_context) : builder(_builder), module(_module), context(_context) {}

void CodeEmittingNodeVisitor::visitLeaf(Leaf *_acceptor)
{
    auto t = _acceptor->token.getType();
    switch (t)
    {
    case Type::continuekw:
        if (continue_stack.empty())
        {
            // Error
        }
        else
        {
            builder->CreateBr(continue_stack.top());
        }
        break;
    case Type::passkw:

        break;
    case Type::breakkw:
        if (break_stack.empty())
        {
            // Error
        }
        else
        {
            builder->CreateBr(continue_stack.top());
        }
        break;

    default:
        // Error как ты сюда попал?
        break;
    }
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitFormalParamsNode(FormalParamsNode *_acceptor)
{
    // Error как ты сюда попал?
}

void CodeEmittingNodeVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    std::vector<llvm::Value *> vec;
    for (auto &i : _acceptor->params)
    {
        i->accept(this);
        vec.push_back(stored_values.front());
        stored_values.pop();
    }
    stored_array = vec;
}

void CodeEmittingNodeVisitor::visitVariableNode(VariableNode *_acceptor)
{
    auto val = getLeafValue(namedValues, builder, _acceptor->chain[0]);
    if (!val)
    {
        // Error
        return;
    }
    stored_values.push(val);
}

void CodeEmittingNodeVisitor::visitCallNode(CallNode *_acceptor)
{
    _acceptor->callable->accept(this);
    auto callable = module->getFunction(stored_values.front()->getName());
    stored_values.pop();
    _acceptor->params->accept(this);
    auto call = builder->CreateCall(callable, stored_array);
    stored_values.push(call);
}

void CodeEmittingNodeVisitor::visitBinaryNode(BinaryNode *_acceptor)
{
    _acceptor->left->accept(this);
    auto left = stored_values.front();
    stored_values.pop();

    _acceptor->right->accept(this);
    auto right = stored_values.front();
    stored_values.pop();

    auto flt = llvm::Type::getFloatTy(*context);
    auto type = llvm::FunctionType::get(flt, {flt}, false);
    auto func = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, "__floor", module);

    llvm::Value *ret;

    if (left->getType()->isPointerTy())
    {
        left = builder->CreateLoad(flt, left, "left");
    }
    if (right->getType()->isPointerTy())
    {
        right = builder->CreateLoad(flt, right, "right");
    }

    switch (_acceptor->op->token.getType())
    {
    case Type::plus:
        ret = builder->CreateFAdd(left, right);
        break;
    case Type::minus:
        ret = builder->CreateFSub(left, right);
        break;
    case Type::star:
        ret = builder->CreateFMul(left, right);
        break;
    case Type::div:
        ret = builder->CreateFDiv(left, right);
        break;
    case Type::mod:
        ret = builder->CreateFRem(left, right);
        break;
    case Type::idiv:
        ret = builder->CreateCall(func, {builder->CreateFDiv(left, right)});
        break;
    case Type::matmul:
        // Error
        break;
    case Type::greater:
        ret = builder->CreateFCmpUGT(left, right);
        break;
    case Type::less:
        ret = builder->CreateFCmpULT(left, right);
        break;
    case Type::equal:
        ret = builder->CreateFCmpUEQ(left, right);
        break;
    case Type::noteq:
        ret = builder->CreateFCmpUNE(left, right);
        break;
    case Type::grequal:
        ret = builder->CreateFCmpUGE(left, right);
        break;
    case Type::lequal:
        ret = builder->CreateFCmpULE(left, right);
        break;
    case Type::andop:
        ret = builder->CreateLogicalAnd(left, right);
        break;
    case Type::orop:
        ret = builder->CreateLogicalOr(left, right);
        break;
    case Type::lshift:
        ret = builder->CreateShl(left, right);
        break;
    case Type::rshift:
        ret = builder->CreateLShr(left, right);
        break;
    case Type::band:
        ret = builder->CreateAnd(left, right);
        break;
    case Type::bor:
        ret = builder->CreateOr(left, right);
        break;
    case Type::xorop:
        ret = builder->CreateXor(left, right);
        break;
    default:
        // Error
        break;
    }
    stored_values.push(ret);
}

void CodeEmittingNodeVisitor::visitUnaryNode(UnaryNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    llvm::Value *left;
    auto name = _acceptor->left->token.getValue();
    if (namedValues.find(name) == namedValues.end())
    {
        auto type = llvm::Type::getFloatTy(*context);
        auto size = llvm::ConstantInt::get(*context, llvm::APInt(32, 1));
        left = builder->CreateAlloca(llvm::Type::getFloatTy(*context), size, name);
        namedValues[name] = left;
    }
    else
    {
        left = namedValues[name];
    }
    _acceptor->right->accept(this);
    auto right = stored_values.front();
    stored_values.pop();
    builder->CreateStore(right, left);
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    _acceptor->return_value->accept(this);
    auto ret_val = stored_values.front();
    stored_values.pop();
    builder->CreateRet(ret_val);
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitBlockNode(BlockNode *_acceptor)
{
    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        stored_values.pop();
    }
}

void CodeEmittingNodeVisitor::visitProgramNode(ProgramNode *_acceptor)
{
    auto intty = llvm::Type::getInt32Ty(*context);
    auto type = llvm::FunctionType::get(intty, false);
    auto main = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, "__pymain", module);
    main_block = llvm::BasicBlock::Create(*context, "__mainbody", main);
    builder->SetInsertPoint(main_block);
    namedValues["__pymain"] = main;
    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        stored_values.pop();
    }
    auto ret = llvm::ConstantInt::get(intty, llvm::APInt(32, 0));
    builder->CreateRet(ret);
}

void CodeEmittingNodeVisitor::visitFunctionNode(FunctionNode *_acceptor)
{
    auto name = _acceptor->id->token.getValue();
    if (namedValues.find(name) != namedValues.end())
    {
        // Error;
        return;
    }
    auto size = _acceptor->formal_params->params.size();
    auto floatty = llvm::Type::getFloatTy(*context);
    std::vector<llvm::Type *> args(size, floatty);
    auto type = llvm::FunctionType::get(floatty, args, false);
    auto func = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, name, module);
    namedValues[name] = func;
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "body", func);
    builder->SetInsertPoint(BB);
    _acceptor->body->accept(this);
    builder->SetInsertPoint(main_block);
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitElseNode(ElseNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitElifNode(ElifNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitIfNode(IfNode *_acceptor)
{
    auto parent = builder->GetInsertBlock()->getParent();
    _acceptor->condition->accept(this);
    auto cond = stored_values.front();
    stored_values.pop();
    llvm::BasicBlock *current_block = builder->GetInsertBlock();
    llvm::BasicBlock *thenb = llvm::BasicBlock::Create(*context, "thenblock", parent);
    std::vector<llvm::BasicBlock *> elif_list;
    llvm::BasicBlock *elseb = llvm::BasicBlock::Create(*context, "elseblock");
    llvm::BasicBlock *merge = llvm::BasicBlock::Create(*context, "mergeblock");
    builder->CreateCondBr(cond, thenb, elseb);

    break_stack.push(merge);

    builder->SetInsertPoint(thenb);
    _acceptor->body->accept(this);
    builder->CreateBr(merge);
    thenb = builder->GetInsertBlock();

    parent->insert(parent->end(), elseb);
    if (_acceptor->next_else)
    {
        _acceptor->next_else->accept(this);
    }
    builder->CreateBr(merge);
    elseb = builder->GetInsertBlock();

    auto curr = _acceptor->next_elif;
    while (curr)
    {
        curr = curr->next_elif;
    }
    // builder->CreatePHI()
}

void CodeEmittingNodeVisitor::visitWhileNode(WhileNode *_acceptor)
{
}

void CodeEmittingNodeVisitor::visitForNode(ForNode *_acceptor)
{
    // Пока генераторы и массивы не реализованы, никаких циклов for
    // Error
}

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

llvm::Value *CodeEmittingNodeVisitor::getLeafValue(Leaf *_leaf)
{
    Type t = _leaf->token.getType();
    std::string val = _leaf->token.getValue();
    if (t == Type::number)
    {
        auto type = llvm::Type::getFloatTy(*context);
        return llvm::ConstantFP::get(type, llvm::StringRef(val));
    }
    if (t == Type::id)
    {
        if (namedValues.top().find(val) == namedValues.top().end())
        {
            return nullptr;
        }
        return namedValues.top().at(val);
    }
    if (t == Type::string)
    {
        return builder->CreateGlobalStringPtr(val);
    }
    return nullptr;
}

void CodeEmittingNodeVisitor::stdinit()
{
    auto floatty = llvm::Type::getFloatTy(*context);
    auto voidty = llvm::Type::getVoidTy(*context);
    auto strty = llvm::Type::getInt8PtrTy(*context);

    auto putfty = llvm::FunctionType::get(voidty, {floatty}, false);
    auto putsty = llvm::FunctionType::get(voidty, {strty}, false);
    auto floorty = llvm::FunctionType::get(floatty, {floatty}, false);

    llvm::Function::Create(floorty, llvm::GlobalValue::ExternalLinkage, "__floor", module);
    llvm::Function::Create(putfty, llvm::GlobalValue::ExternalLinkage, "__putf", module);
    llvm::Function::Create(putsty, llvm::GlobalValue::ExternalLinkage, "__puts", module);
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

void CodeEmittingNodeVisitor::visitFormalParamsNode(FormalParamsNode *)
{
    // Error как ты сюда попал?
}

void CodeEmittingNodeVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    stored_array.clear();
    for (auto &i : _acceptor->params)
    {
        i->accept(this);
        auto val = stored_values.front();
        stored_values.pop();
        if (val->getType()->isPointerTy())
        {
            stored_array.push_back(builder->CreateLoad(llvm::Type::getFloatTy(*context), val));
        }
        else
        {
            stored_array.push_back(val);
        }
    }
}

void CodeEmittingNodeVisitor::visitCallNode(CallNode *_acceptor)
{
    // auto funcname = _acceptor->callable->accept(this);
    auto funcname = "printf";
    auto callable = module->getFunction(funcname);
    // auto g = callable->getName();
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

    auto floatty = llvm::Type::getFloatTy(*context);

    llvm::Value *ret;

    if (left->getType()->isPointerTy())
    {
        left = builder->CreateLoad(floatty, left, "left");
    }
    if (right->getType()->isPointerTy())
    {
        right = builder->CreateLoad(floatty, right, "right");
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
        ret = builder->CreateCall(module->getFunction("__floor"), {builder->CreateFDiv(left, right)});
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

void CodeEmittingNodeVisitor::visitUnaryNode(UnaryNode *)
{
}

void CodeEmittingNodeVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    llvm::Value *left;
    auto name = _acceptor->left->token.getValue();
    if (namedValues.top().find(name) == namedValues.top().end())
    {
        // auto type = llvm::Type::getFloatTy(*context);
        auto size = llvm::ConstantInt::get(*context, llvm::APInt(32, 1));
        left = builder->CreateAlloca(llvm::Type::getFloatTy(*context), size, name);
        namedValues.top().insert({name, left});
    }
    else
    {
        left = namedValues.top().at(name);
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
    stdinit();

    namedValues.emplace();

    auto intty = llvm::Type::getInt32Ty(*context);
    auto type = llvm::FunctionType::get(intty, false);
    auto main = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, "__pymain", module);
    main_block = llvm::BasicBlock::Create(*context, "__mainbody", main);
    builder->SetInsertPoint(main_block);
    namedValues.top().insert({"__pymain", main});
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
    auto floatty = llvm::Type::getFloatTy(*context);
    auto name = _acceptor->id->token.getValue();
    auto paramsize = llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*context), llvm::APInt(32, 1));
    if (namedValues.top().find(name) != namedValues.top().end())
    {
        // Error;
        return;
    }
    namedValues.emplace(namedValues.top());
    for (auto &i : _acceptor->formal_params->params)
    {
        namedValues.top().insert({i->token.getValue(), builder->CreateAlloca(floatty, paramsize)});
    }
    auto size = _acceptor->formal_params->params.size();
    std::vector<llvm::Type *> args(size, floatty);
    auto type = llvm::FunctionType::get(floatty, args, false);
    auto func = llvm::Function::Create(type, llvm::GlobalValue::InternalLinkage, name, module);
    namedValues.top().insert({name, func});
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "body", func);
    builder->SetInsertPoint(BB);
    _acceptor->body->accept(this);
    namedValues.pop();
    builder->SetInsertPoint(main_block);
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitElseNode(ElseNode *)
{
}

void CodeEmittingNodeVisitor::visitElifNode(ElifNode *_acceptor)
{
    auto parent = builder->GetInsertBlock()->getParent();
    _acceptor->condition->accept(this);
    auto cond = stored_values.front();
    stored_values.pop();
    // llvm::BasicBlock *current_block = builder->GetInsertBlock();
    llvm::BasicBlock *thenb = llvm::BasicBlock::Create(*context, "thenblock", parent);
    llvm::BasicBlock *elseb = llvm::BasicBlock::Create(*context, "elseblock");
    builder->CreateCondBr(cond, thenb, elseb);

    builder->SetInsertPoint(thenb);
    _acceptor->body->accept(this);
    builder->CreateBr(merge_stack.top());

    parent->insert(parent->end(), elseb);
    builder->SetInsertPoint(elseb);
}

void CodeEmittingNodeVisitor::visitIfNode(IfNode *_acceptor)
{
    auto currblock = builder->GetInsertBlock();
    auto parent = builder->GetInsertBlock()->getParent();
    _acceptor->condition->accept(this);
    auto cond = stored_values.front();
    stored_values.pop();
    // llvm::BasicBlock *current_block = builder->GetInsertBlock();
    llvm::BasicBlock *thenb = llvm::BasicBlock::Create(*context, "thenblock", parent);
    llvm::BasicBlock *elseb = llvm::BasicBlock::Create(*context, "elseblock");
    llvm::BasicBlock *merge = llvm::BasicBlock::Create(*context, "mergeblock");
    builder->CreateCondBr(cond, thenb, elseb);

    merge_stack.push(merge);

    builder->SetInsertPoint(thenb);
    _acceptor->body->accept(this);
    builder->CreateBr(merge);
    thenb = builder->GetInsertBlock();

    parent->insert(parent->end(), elseb);
    builder->SetInsertPoint(elseb);

    auto curr = _acceptor->next_elif;
    if (curr)
    {
        for (;;)
        {
            curr->accept(this);
            if (curr->next_elif)
                curr = curr->next_elif;
            else
                break;
        }

        if (curr->next_else)
        {
            curr->next_else->body->accept(this);
        }
    }

    if (_acceptor->next_else)
    {
        _acceptor->next_else->body->accept(this);
    }

    builder->CreateBr(merge);

    parent->insert(parent->end(), merge);
    builder->SetInsertPoint(merge);
    if (currblock == main_block)
    {
        main_block = merge;
    }
    merge_stack.pop();

    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitWhileNode(WhileNode *_acceptor)
{
    auto currblock = builder->GetInsertBlock();
    auto parent = builder->GetInsertBlock()->getParent();
    // llvm::BasicBlock *current_block = builder->GetInsertBlock();
    llvm::BasicBlock *condb = llvm::BasicBlock::Create(*context, "condb", parent);
    llvm::BasicBlock *bodyb = llvm::BasicBlock::Create(*context, "bodyb", parent);
    llvm::BasicBlock *afterb = llvm::BasicBlock::Create(*context, "afterb");

    builder->CreateBr(condb);

    builder->SetInsertPoint(condb);
    _acceptor->condition->accept(this);
    auto cond = stored_values.front();
    stored_values.pop();

    builder->CreateCondBr(cond, bodyb, afterb);

    break_stack.push(afterb);
    continue_stack.push(condb);

    builder->SetInsertPoint(bodyb);
    _acceptor->body->accept(this);
    builder->CreateBr(condb);

    parent->insert(parent->end(), afterb);
    builder->SetInsertPoint(afterb);

    if (currblock == main_block)
    {
        main_block = afterb;
    }

    break_stack.pop();
    continue_stack.pop();
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitForNode(ForNode *)
{
    // Пока генераторы и массивы не реализованы, никаких циклов for
    // Error
}

void CodeEmittingNodeVisitor::visitListNode(ListNode *)
{
}

void CodeEmittingNodeVisitor::visitTypeNode(TypeNode *)
{
}

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
#include <pycom/utility/Types.hpp>

#include <unordered_map>
#include <tuple>

const std::unordered_map<std::string, CodeEmittingNodeVisitor::my_type> CodeEmittingNodeVisitor::typemap = {
    {INTEGER_TYPE, CodeEmittingNodeVisitor::my_type::int_type},
    {STRING_TYPE, CodeEmittingNodeVisitor::my_type::str_type},
    {BOOL_TYPE, CodeEmittingNodeVisitor::my_type::bool_type},
    {VOID_TYPE, CodeEmittingNodeVisitor::my_type::void_type}};

std::tuple<CodeEmittingNodeVisitor::my_type, llvm::Type *> CodeEmittingNodeVisitor::__str_to_type(const std::string &_str)
{
    auto t = typemap.find(_str);
    my_type tt;
    if (t != typemap.end())
    {
        tt = t->second;
    }
    else
    {
        tt = typemap.find(VOID_TYPE)->second;
    }

    switch (tt)
    {
    case my_type::int_type:
        return std::make_tuple(tt, llvm::Type::getFloatTy(*context));
        break;
    case my_type::bool_type:
        return std::make_tuple(tt, llvm::Type::getInt8Ty(*context));
        break;
    case my_type::str_type:
        return std::make_tuple(tt, llvm::Type::getInt8PtrTy(*context));
        break;
    default:
        break;
    }
    return std::make_tuple(tt, nullptr);
}

std::tuple<CodeEmittingNodeVisitor::my_type, llvm::Type *, bool> CodeEmittingNodeVisitor::str_to_type(const std::string &_str)
{
    auto pos = _str.find(LIST_TYPE);
    if (pos == _str.npos)
    {
        auto g = __str_to_type(_str);
        return std::make_tuple(std::get<0>(g), std::get<1>(g), false);
    }
    else
    {
        pos += 4;
        auto type_ptr = _str.c_str() + pos;
        auto g = __str_to_type(type_ptr);
        return std::make_tuple(std::get<0>(g), std::get<1>(g), true);
    }
}

void CodeEmittingNodeVisitor::visitFormalParamsNode(FormalParamsNode *) {}
void CodeEmittingNodeVisitor::visitElseNode(ElseNode *) {}
void CodeEmittingNodeVisitor::visitTypeNode(TypeNode *) {}

llvm::Value *CodeEmittingNodeVisitor::getLeafValue(Leaf *_leaf)
{
    lastrow = _leaf->token.getRow();
    lastpos = _leaf->token.getPos();

    Type t = _leaf->token.getType();
    std::string val = _leaf->token.getValue();
    llvm::Value *v;
    std::tuple<CodeEmittingNodeVisitor::my_type, llvm::Type *, bool> tt;
    auto voidptrty = llvm::PointerType::get(*context, 0);

    llvm::Type *type;
    switch (t)
    {
    case Type::number:
        type = llvm::Type::getFloatTy(*context);
        return llvm::ConstantFP::get(type, llvm::StringRef(val));
        break;
    case Type::id:
        if (namedValues.top().find(val) == namedValues.top().end())
        {
            return nullptr;
        }
        v = namedValues.top().at(val);
        tt = str_to_type(*_leaf->type);
        if (std::get<2>(tt)) // is list
        {
            return builder->CreateCall(module->getFunction(LISTNEXT),
                                       {builder->CreateLoad(voidptrty, v)}); // return pointer to list struct
        }
        else
        {
            if (std::get<1>(tt)) // pointer to type
            {
                switch (std::get<0>(tt)) // my_type
                {
                case my_type::int_type:
                    return (builder->CreateLoad(std::get<1>(tt), v)); // load int variable
                    break;
                case my_type::str_type:
                    return (builder->CreateLoad(std::get<1>(tt), v)); // load pointer to string
                    break;
                case my_type::bool_type:
                    return (builder->CreateLoad(std::get<1>(tt), v)); // load bool variable
                    break;
                default:
                    break;
                }
            }
        }
        break;
    case Type::string:
        return builder->CreateGlobalStringPtr(val);
        break;
    default:
        return nullptr;
        break;
    }
    return nullptr;
}

CodeEmittingNodeVisitor::CodeEmittingNodeVisitor(
    llvm::IRBuilder<> *_builder,
    llvm::Module *_module,
    llvm::LLVMContext *_context) : builder(_builder),
                                   module(_module),
                                   context(_context) {}

void CodeEmittingNodeVisitor::visitLeaf(Leaf *_acceptor)
{
    auto &t = _acceptor->token;

    lastrow = t.getRow();
    lastpos = t.getPos();

    llvm::Value *v;
    switch (t.getType())
    {
    case Type::continuekw:
        if (continue_stack.empty())
        {
            error(
                "Continue statement outside loop.\n"
                "Occured at row " +
                std::to_string(t.getRow()) +
                "position " + std::to_string(t.getPos()) + "\n");
        }
        else
        {
            builder->CreateBr(continue_stack.top());
            block_ended = true;
        }
        break;
    case Type::breakkw:
        if (break_stack.empty())
        {
            error(
                "Break statement outside loop.\n"
                "Occured at row " +
                std::to_string(t.getRow()) +
                "position " + std::to_string(t.getPos()) + "\n");
        }
        else
        {
            builder->CreateBr(break_stack.top());
            block_ended = true;
        }
        break;
    case Type::id:
    case Type::string:
    case Type::number:
        v = getLeafValue(_acceptor);
        stored_values.push(v);
        break;

    default:
        stored_values.push(nullptr);
        break;
    }
}

void CodeEmittingNodeVisitor::visitActualParamsNode(ActualParamsNode *_acceptor)
{
    for (auto &i : _acceptor->params)
    {
        i->accept(this);
        auto val = stored_values.front();
        stored_values.pop();
        stored_array.push_back(val);
    }
}

void CodeEmittingNodeVisitor::visitCallNode(CallNode *_acceptor)
{
    auto funcname = _acceptor->callable.getValue();
    auto callable = module->getFunction(funcname);
    _acceptor->params->accept(this);
    auto size = callable->arg_size();
    std::vector<llvm::Value *> actual(stored_array.end() - size, stored_array.end());
    stored_array.resize(stored_array.size() - size);
    auto call = builder->CreateCall(callable, actual);
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

    llvm::Value *ret;

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
        ret = builder->CreateCall(module->getFunction(FLOOR), {builder->CreateFDiv(left, right)});
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
    _acceptor->operand->accept(this);
    auto val = stored_values.front();
    stored_values.pop();

    auto op = _acceptor->op->token.getType();
    switch (op)
    {
    case Type::minus:
        builder->CreateFNeg(val);
        break;
    case Type::plus:
        break;
    case Type::notop:
        builder->CreateNot(val);
        break;

    default:
        break;
    }
}

void CodeEmittingNodeVisitor::visitAssignmentNode(AssignmentNode *_acceptor)
{
    llvm::Value *left;

    _acceptor->right->accept(this);
    auto right = stored_values.front();
    stored_values.pop();
    auto tt = std::get<1>(str_to_type(*_acceptor->right->type));

    auto name = _acceptor->left->token.getValue();
    if (namedValues.top().find(name) == namedValues.top().end())
    {
        // auto size = llvm::ConstantInt::get(*context, llvm::APInt(32, 1));
        left = builder->CreateAlloca(tt, nullptr, name);
        namedValues.top().insert({name, left});
    }
    else
    {
        left = namedValues.top().at(name);
    }
    builder->CreateStore(right, left);
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitReturnNode(ReturnNode *_acceptor)
{
    block_ended = true;

    _acceptor->return_value->accept(this);
    auto ret_val = stored_values.front();
    stored_values.pop();

    if (return_value && return_block)
    {
        builder->CreateStore(ret_val, return_value);
        builder->CreateBr(return_block);
    }
    else
    {
        error(std::string("Return statement outside function\n"
                          "Occured at row: ") +
              std::to_string(lastrow) + " pos: " + std::to_string(lastpos));
    }
    stored_values.push(nullptr);
}

void CodeEmittingNodeVisitor::visitBlockNode(BlockNode *_acceptor)
{
    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        stored_values.pop();
        if (block_ended)
        {
            return;
        }
    }
}

void CodeEmittingNodeVisitor::visitProgramNode(ProgramNode *_acceptor)
{
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
    auto name = _acceptor->id->token.getValue();
    // auto paramsize = llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*context), llvm::APInt(32, 1));
    if (namedValues.top().find(name) != namedValues.top().end())
    {
        // Error;
        return;
    }
    namedValues.emplace(namedValues.top());

    auto size = _acceptor->formal_params->params.size();
    std::vector<llvm::Type *> args;
    args.reserve(size);
    for (auto &i : _acceptor->formal_params->types)
    {
        auto tt = std::get<1>(str_to_type(i->token.getValue()));
        args.push_back(tt);
    }
    auto returntype = std::get<1>(str_to_type(_acceptor->return_type->token.getValue()));
    auto type = llvm::FunctionType::get(returntype, args, false);
    auto func = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, name, module);

    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "body", func);
    return_block = llvm::BasicBlock::Create(*context, "retblock", func);
    builder->SetInsertPoint(BB);

    return_value = builder->CreateAlloca(returntype, nullptr, "__retval");

    auto arg_iter = func->args().begin();
    auto type_iter = _acceptor->formal_params->types.begin();

    for (auto &i : _acceptor->formal_params->params)
    {
        auto alloc = builder->CreateAlloca(std::get<1>(str_to_type((*(type_iter++))->token.getValue())), nullptr, i->token.getValue());
        builder->CreateStore(arg_iter++, alloc);
        namedValues.top().insert({i->token.getValue(), alloc});
    }
    namedValues.top().insert({name, func});
    _acceptor->body->accept(this);
    namedValues.pop();

    if (!block_ended)
    {
        builder->CreateBr(return_block);
    }

    builder->SetInsertPoint(return_block);
    builder->CreateRet(builder->CreateLoad(returntype, return_value));
    builder->SetInsertPoint(main_block);

    return_block = nullptr;
    return_value = nullptr;
    block_ended = false;
    stored_values.push(nullptr);
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
    if (!block_ended)
    {
        builder->CreateBr(merge_stack.top());
    }
    block_ended = false;

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
    if (!block_ended)
    {
        builder->CreateBr(merge);
    }
    block_ended = false;
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

    if (!block_ended)
    {
        builder->CreateBr(merge);
    }
    block_ended = false;

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

    if(!block_ended)
    {
        builder->CreateBr(condb);
    }
    block_ended = false;

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

void CodeEmittingNodeVisitor::visitForNode(ForNode *_acceptor)
{
    auto &token = _acceptor->iterator->token;
    auto currblock = builder->GetInsertBlock();
    auto parent = builder->GetInsertBlock()->getParent();
    llvm::BasicBlock *initb = llvm::BasicBlock::Create(*context, "initb", parent);
    llvm::BasicBlock *condb = llvm::BasicBlock::Create(*context, "condb", parent);
    llvm::BasicBlock *bodyb = llvm::BasicBlock::Create(*context, "bodyb", parent);
    llvm::BasicBlock *nextb = llvm::BasicBlock::Create(*context, "nextb", parent);
    llvm::BasicBlock *afterb = llvm::BasicBlock::Create(*context, "afterb");
    auto voidptrty = llvm::PointerType::get(*context, 0);

    auto nextfunc = module->getFunction(LISTNEXT);
    auto loadfunc = module->getFunction(LISTLOAD);

    auto list_elem_type = std::get<1>(str_to_type(*_acceptor->condition->type));

    builder->CreateBr(initb);

    builder->SetInsertPoint(initb); // loop init
    auto elem = builder->CreateAlloca(list_elem_type, nullptr, token.getValue());
    namedValues.top().insert({token.getValue(), elem}); // store loop variable in symbol table
    _acceptor->condition->accept(this);
    auto iter = builder->CreateAlloca(voidptrty, nullptr, "iter"); // **struct
    builder->CreateStore(stored_values.front(), iter);
    stored_values.pop();
    builder->CreateBr(condb);

    builder->SetInsertPoint(condb); // condition
    auto cond = builder->CreateIsNull(builder->CreateLoad(voidptrty, iter));
    builder->CreateCondBr(cond, bodyb, afterb);

    break_stack.push(afterb);
    continue_stack.push(nextb);

    builder->SetInsertPoint(bodyb);                                                    // loop body
    auto load = builder->CreateCall(loadfunc, {builder->CreateLoad(voidptrty, iter)}); // pointer to list element data
    builder->CreateStore(builder->CreateLoad(list_elem_type, load), elem);             // store list element data to loop variable
    _acceptor->body->accept(this);
    if(!block_ended)
    {
        builder->CreateBr(nextb);
    }
    block_ended = false;

    builder->SetInsertPoint(nextb);

    auto curr = builder->CreateLoad(voidptrty, iter);      // *struct
    auto nextcall = builder->CreateCall(nextfunc, {curr}); // get next *struct
    builder->CreateStore(nextcall, iter);                  // store next to iterator
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

void CodeEmittingNodeVisitor::visitListNode(ListNode *_acceptor)
{
    auto create = module->getFunction(LISTCREATE);
    llvm::Function *push = nullptr;

    auto listelemtype = std::get<0>(str_to_type(*_acceptor->type));
    switch (listelemtype)
    {
    case my_type::int_type:
        push = module->getFunction(LISTPUSHINT);
        break;
    case my_type::bool_type:
        push = module->getFunction(LISTPUSHBOOL);
        break;
    case my_type::str_type:
        push = module->getFunction(LISTPUSHSTR);
        break;
    default:
        error("Unexpected type\nOccured at row: " + std::to_string(lastrow) + " pos: " + std::to_string(lastpos));
        break;
    }

    auto start = builder->CreateCall(create, {});

    for (auto &i : _acceptor->children)
    {
        i->accept(this);
        builder->CreateCall(push, {start, stored_values.front()});
        stored_values.pop();
    }
    stored_values.push(start);
}

void CodeEmittingNodeVisitor::setEM(ErrorManagerInterface *_em)
{
    em = _em;
}

void CodeEmittingNodeVisitor::error(const std::string &_str)
{
    em->error(_str);
}

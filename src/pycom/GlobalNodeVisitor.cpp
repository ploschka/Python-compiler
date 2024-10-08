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

#include <pycom/codegen/GlobalNodeVisitor.hpp>
#include <pycom/AST/ASTNode.hpp>
#include <pycom/utility/Types.hpp>

void GlobalNodeVisitor::stdinit()
{
    auto floatty = llvm::Type::getFloatTy(*context);
    auto voidty = llvm::Type::getVoidTy(*context);
    auto strty = llvm::Type::getInt8PtrTy(*context);
    auto boolty = llvm::Type::getInt8Ty(*context);
    auto voidptrty = llvm::PointerType::get(*context, 0);

    auto putfty = llvm::FunctionType::get(voidty, {floatty}, false);
    auto putsty = llvm::FunctionType::get(voidty, {strty}, false);
    auto floorty = llvm::FunctionType::get(floatty, {floatty}, false);
    auto lloadty = llvm::FunctionType::get(voidptrty, {voidptrty}, false);
    auto lstorety = llvm::FunctionType::get(voidty, {voidptrty}, false);
    auto lnext = llvm::FunctionType::get(voidptrty, {voidptrty}, false);
    auto lpushi = llvm::FunctionType::get(voidty, {voidptrty, floatty}, false);
    auto lpushs = llvm::FunctionType::get(voidty, {voidptrty, strty}, false);
    auto lpushb = llvm::FunctionType::get(voidty, {voidptrty, boolty}, false);
    auto lcreate = llvm::FunctionType::get(voidptrty, {}, false);

    llvm::Function::Create(floorty, llvm::GlobalValue::ExternalLinkage, FLOOR, module);
    llvm::Function::Create(putfty, llvm::GlobalValue::ExternalLinkage, PUTF, module);
    llvm::Function::Create(putsty, llvm::GlobalValue::ExternalLinkage, PUTS, module);
    llvm::Function::Create(lloadty, llvm::GlobalValue::ExternalLinkage, LISTLOAD, module);
    llvm::Function::Create(lstorety, llvm::GlobalValue::ExternalLinkage, LISTSTORE, module);
    llvm::Function::Create(lnext, llvm::GlobalValue::ExternalLinkage, LISTNEXT, module);
    llvm::Function::Create(lpushi, llvm::GlobalValue::ExternalLinkage, LISTPUSHINT, module);
    llvm::Function::Create(lpushs, llvm::GlobalValue::ExternalLinkage, LISTPUSHSTR, module);
    llvm::Function::Create(lpushb, llvm::GlobalValue::ExternalLinkage, LISTPUSHBOOL, module);
    llvm::Function::Create(lcreate, llvm::GlobalValue::ExternalLinkage, LISTCREATE, module);
}

void GlobalNodeVisitor::error(const std::string &_str)
{
    if (em)
    {
        em->error(_str);
    }
}

void GlobalNodeVisitor::setEM(ErrorManagerInterface *_em)
{
    em = _em;
}

GlobalNodeVisitor::GlobalNodeVisitor(
    llvm::IRBuilder<> *_builder,
    llvm::Module *_module,
    llvm::LLVMContext *_context) : builder(_builder),
                                   module(_module),
                                   context(_context) {}

void GlobalNodeVisitor::visitFunctionNode(FunctionNode *)
{
    // auto floatty = llvm::Type::getFloatTy(*context);
    // auto name = _acceptor->id->token.getValue();
    // // auto paramsize = llvm::Constant::getIntegerValue(llvm::Type::getInt32Ty(*context), llvm::APInt(32, 1));
    // if (namedValues.top().find(name) != namedValues.top().end())
    // {
    //     // Error;
    //     return;
    // }

    // namedValues.emplace(namedValues.top());

    // auto size = _acceptor->formal_params->params.size();
    // std::vector<llvm::Type *> args(size, floatty);
    // auto type = llvm::FunctionType::get(floatty, args, false);
    // auto func = llvm::Function::Create(type, llvm::GlobalValue::ExternalLinkage, name, module);

    // llvm::BasicBlock *BB = llvm::BasicBlock::Create(*context, "body", func);
    // builder->SetInsertPoint(BB);

    // auto arg_iter = func->args().begin();

    // for (auto &i : _acceptor->formal_params->params)
    // {
    //     auto alloc = builder->CreateAlloca(floatty, nullptr, i->token.getValue());
    //     builder->CreateStore(arg_iter++, alloc);
    //     namedValues.top().insert({i->token.getValue(), alloc});
    // }
    // namedValues.top().insert({name, func});
    // _acceptor->body->accept(this);
    // namedValues.pop();
    // builder->SetInsertPoint(main_block);
    // stored_values.push(nullptr);
}

void GlobalNodeVisitor::visitProgramNode(ProgramNode *)
{
    stdinit();
}

void GlobalNodeVisitor::visitLeaf(Leaf *) {}
void GlobalNodeVisitor::visitTypeNode(TypeNode *) {}
void GlobalNodeVisitor::visitFormalParamsNode(FormalParamsNode *) {}
void GlobalNodeVisitor::visitActualParamsNode(ActualParamsNode *) {}
void GlobalNodeVisitor::visitCallNode(CallNode *) {}
void GlobalNodeVisitor::visitBinaryNode(BinaryNode *) {}
void GlobalNodeVisitor::visitUnaryNode(UnaryNode *) {}
void GlobalNodeVisitor::visitAssignmentNode(AssignmentNode *) {}
void GlobalNodeVisitor::visitReturnNode(ReturnNode *) {}
void GlobalNodeVisitor::visitBlockNode(BlockNode *) {}
void GlobalNodeVisitor::visitElseNode(ElseNode *) {}
void GlobalNodeVisitor::visitElifNode(ElifNode *) {}
void GlobalNodeVisitor::visitIfNode(IfNode *) {}
void GlobalNodeVisitor::visitWhileNode(WhileNode *) {}
void GlobalNodeVisitor::visitForNode(ForNode *) {}
void GlobalNodeVisitor::visitListNode(ListNode *) {}

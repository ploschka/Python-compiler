#include <pycom/codegen/CodeGenerator.hpp>

void CodeGenerator::generate(AST *_ast)
{
    _ast->accept(globalvisitor.get());
    _ast->accept(codevisitor.get());
}

CodeGenerator::CodeGenerator(llvm::IRBuilder<> *_builder,
                             llvm::Module *_module,
                             llvm::LLVMContext *_context)
{
    globalvisitor = std::make_unique<GlobalNodeVisitor>(_builder, _module, _context);
    codevisitor = std::make_unique<CodeEmittingNodeVisitor>(_builder, _module, _context);
}

void CodeGenerator::setEM(ErrorManagerInterface *_em)
{
    globalvisitor->setEM(_em);
    codevisitor->setEM(_em);
}

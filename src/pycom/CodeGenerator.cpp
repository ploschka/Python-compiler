#include <pycom/codegen/CodeGenerator.hpp>

void CodeGenerator::generate(AST *_ast)
{
    _ast->accept(visitor.get());
}

CodeGenerator::CodeGenerator(llvm::IRBuilder<> *_builder,
                             llvm::Module *_module,
                             llvm::LLVMContext *_context)
{
    visitor = std::make_unique<CodeEmittingNodeVisitor>(_builder, _module, _context);
}

void CodeGenerator::setEM(ErrorManagerInterface* _em)
{
    visitor->setEM(_em);
}

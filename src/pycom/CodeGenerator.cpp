#include <pycom/codegen/CodeGenerator.hpp>

void CodeGenerator::generate(AST *_ast)
{
    _ast->accept(visitor.get());
}

CodeGenerator::CodeGenerator(llvm::IRBuilder<> *_builder,
                             llvm::Module *_module,
                             llvm::LLVMContext *_context,
                             block_map_t *_map)
{
    visitor = std::make_unique<CodeEmittingNodeVisitor>(_builder, _module, _context, _map);
}

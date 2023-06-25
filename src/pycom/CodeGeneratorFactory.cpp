#include <pycom/factory/CodeGenFactory.hpp>
#include <pycom/codegen/CodeGenerator.hpp>

std::unique_ptr<CodeGeneratorInterface> CodeGenFactory::create(llvm::IRBuilder<> *_builder, llvm::Module *_module, llvm::LLVMContext *_context)
{
    return std::make_unique<CodeGenerator>(_builder, _module, _context);
}

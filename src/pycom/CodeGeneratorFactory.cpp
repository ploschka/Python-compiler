#include <pycom/factory/CodeGenFactory.hpp>
#include <pycom/codegen/CodeGenerator.hpp>

std::unique_ptr<CodeGeneratorInterface> CodeGenFactory::create(){
    return std::make_unique<CodeGenerator>();
}

#include <pycom/codegen/CodeGenerator.hpp>

void CodeGenerator::generate(AST *_ast)
{
    _ast->accept(visitor.get());
}

CodeGenerator::CodeGenerator(std::ostream& _stream)
{
    visitor = std::make_unique<CodeEmittingNodeVisitor>(_stream);
}

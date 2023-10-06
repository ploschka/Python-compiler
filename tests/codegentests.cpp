#include <pycom/codegen/CodeGenerator.hpp>
#include <gtest/gtest.h>
#include <fstream>
#include <string>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Verifier.h>

static auto context = llvm::LLVMContext();
static auto builder = llvm::IRBuilder<>(context);
static auto module = llvm::Module("pycom", context);
static auto codegen = std::make_unique<CodeGenerator>(&builder, &module, &context);

TEST(CodeGeneratorTest, VariableConstantAssignment)
{
    std::string out;
    std::string pattern(
        "; ModuleID = 'pycom'\n"
        "source_filename = \"pycom\"\n\n"

        "declare float @__floor(float) addrspace(1)\n\n"

        "declare void @__putf(float) addrspace(1)\n\n"

        "declare void @__puts(ptr) addrspace(1)\n\n"

        "define i32 @__pymain() addrspace(1) {\n"
        "__mainbody:\n"
        "  %a = alloca float, align 4, addrspace(21848)\n  store float 1.000000e+01, ptr addrspace(21848) % a, align 4 ret i32 0\n"
        "}\n");
    
    llvm::raw_string_ostream str(out);
    auto root = new ProgramNode({new AssignmentNode(
        new Leaf(
            Token("a", Type::id)),
        new VariableNode({new Leaf(
            Token("10", Type::number))}))});

    codegen->generate(new AST(root));
    module.print(str, nullptr);
    ASSERT_EQ(out, pattern);
}

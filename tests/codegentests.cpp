#include <pycom/factory/CodeGenFactory.hpp>
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


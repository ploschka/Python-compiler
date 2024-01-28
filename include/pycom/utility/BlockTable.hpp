#pragma once

#include <llvm/IR/BasicBlock.h>

#include <map>
#include <tuple>

#include <pycom/AST/ASTNodePublic.hpp>

typedef std::tuple<llvm::BasicBlock *, BlockNode *> block_pair_t;
typedef std::map<unsigned int, block_pair_t> block_map_t;

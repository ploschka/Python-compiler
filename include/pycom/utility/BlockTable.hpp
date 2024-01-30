#pragma once

#include <llvm/IR/BasicBlock.h>
#include <pycom/semanalyzer/Symbol.hpp>

#include <map>
#include <tuple>
#include <memory>

#include <pycom/AST/ASTNodePublic.hpp>

typedef std::map<std::string, Symbol> localtable_t;
typedef std::tuple<std::shared_ptr<localtable_t>> block_pair_t;
typedef std::map<BlockNode *, block_pair_t> block_map_t;

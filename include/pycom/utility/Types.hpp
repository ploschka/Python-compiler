#pragma once

#include <set>
#include <string>
#include <map>
#include <vector>
#include <functional>

typedef std::set<std::string> typeset_t;
typedef typeset_t::const_iterator type_t;
typedef std::pair<type_t, std::vector<type_t>> func_pair_t;
typedef std::map<std::string, func_pair_t> func_map_t;
typedef std::function<void *(void *)> error_handler_t;

static const char *const STRING_TYPE = "str";
static const char *const INTEGER_TYPE = "int";
static const char *const VOID_TYPE = "void";
static const char *const BOOL_TYPE = "bool";
static const char *const LIST_TYPE = "list";

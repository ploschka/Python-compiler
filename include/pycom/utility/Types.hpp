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

static std::string STRING_TYPE = "str";
static std::string INTEGER_TYPE = "int";
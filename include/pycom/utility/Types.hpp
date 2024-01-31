#pragma once

#include <set>
#include <string>
#include <map>
#include <vector>

typedef std::set<std::string> typeset_t;
typedef typeset_t::const_iterator type_t;
typedef std::pair<type_t, std::vector<type_t>> func_pair_t;
typedef std::map<std::string, func_pair_t> func_map_t;
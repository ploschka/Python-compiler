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

static const char *const PUTS = "puts";
static const char *const PUTF = "putf";
static const char *const FLOOR = "floor";
static const char *const LISTPUSH = "list_push";
static const char *const LISTLOAD = "__list_load";
static const char *const LISTSTORE = "__list_store";
static const char *const LISTNEXT = "__list_next";
static const char *const LISTCREATE = "__list_create";
static const char *const LISTINSERT = "__list_insert";

#include <pycom/semanalyzer/Symbol.hpp>

Symbol::Symbol(Token _token, std::set<std::string>::const_iterator _type) : token(_token), type(_type) {}

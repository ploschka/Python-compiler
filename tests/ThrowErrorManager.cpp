#include "ThrowErrorManager.hpp"
#include <stdexcept>
#include <iostream>

void ThrowErrorManager::error(const std::string &_str, void *)
{
    std::cerr << _str << std::endl;
    throw std::runtime_error(_str);
}

void ThrowErrorManager::error(const char *_str, void *)
{
    std::cerr << _str << std::endl;
    throw std::runtime_error(_str);
}

void ThrowErrorManager::registerHandler(error_handler_t){};

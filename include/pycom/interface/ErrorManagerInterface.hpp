#pragma once

#include <functional>
#include <string>
#include <pycom/utility/Types.hpp>

class ErrorManagerInterface
{
public:
    virtual void error(const std::string &_str, void *_arg = nullptr) = 0;
    virtual void error(const char *_str, void *_arg = nullptr) = 0;
    virtual void registerHandler(error_handler_t _func) = 0;
    virtual ~ErrorManagerInterface() = default;
};

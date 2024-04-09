#pragma once

#include <string>
#include <pycom/utility/Types.hpp>

class ErrorManagerInterface
{
public:
    virtual void error(const std::string &_str, void *_arg = nullptr) = 0;
    virtual void registerHandler(error_handler_t _func) = 0;
    virtual bool errorOccured() = 0;
    virtual void clear() = 0;
    virtual ~ErrorManagerInterface() = default;
};

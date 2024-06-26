#pragma once

#include <pycom/interface/ErrorManagerInterface.hpp>

class ThrowErrorManager : public ErrorManagerInterface
{
public:
    void error(const std::string &_str, void *_arg = nullptr);
    void registerHandler(error_handler_t _func);
    bool errorOccured();
    void clear();
};

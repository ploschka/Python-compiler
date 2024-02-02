#pragma once

#include <pycom/interface/ErrorManagerInterface.hpp>

#include <ostream>

class ErrorManager : public ErrorManagerInterface
{
private:
    std::ostream &stream;
    error_handler_t handler;

public:
    void error(const std::string &_str, void *_arg = nullptr);
    void error(const char *_str, void *_arg = nullptr);
    void registerHandler(error_handler_t _func);
    ErrorManager(std::ostream &_stream);
};

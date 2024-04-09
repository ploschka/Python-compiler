#pragma once

#include <pycom/interface/ErrorManagerInterface.hpp>

#include <ostream>

class ErrorManager : public ErrorManagerInterface
{
private:
    std::ostream &stream;
    error_handler_t handler;
    unsigned int error_code;

public:
    void error(const std::string &_str, void *_arg = nullptr);
    void registerHandler(error_handler_t _func);
    bool errorOccured();
    void clear();
    ErrorManager(std::ostream &_stream);
};

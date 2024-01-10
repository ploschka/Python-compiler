#pragma once

#include <pycom/interface/ErrorManagerInterface.hpp>

#include <ostream>

class ErrorManager : public ErrorManagerInterface
{
private:
    std::ostream &stream;

public:
    void error(const char *_str);
    void error_exit(const char *_str, int _code);
    ErrorManager(std::ostream &_stream);
};

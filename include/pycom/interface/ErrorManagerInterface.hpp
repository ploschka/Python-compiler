#pragma once

class ErrorManagerInterface
{
public:
    virtual void error(const char *_str, int _code) = 0;
    virtual ~ErrorManagerInterface() = default;
};

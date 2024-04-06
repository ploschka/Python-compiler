#pragma once

#include <pycom/interface/ErrorManagerInterface.hpp>

class ErrorEmitterInterface
{
public:
    virtual void setEM(ErrorManagerInterface *_em) = 0;
};

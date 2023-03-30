#pragma once
#include <pycom/interface/LexemeInterface.hpp>
#include <memory>

class LexemeFactory
{
public:
    std::unique_ptr<LexemeInterface> create() const;
};
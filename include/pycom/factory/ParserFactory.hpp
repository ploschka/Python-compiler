#pragma once
#include <pycom/interface/ParserInterface.hpp>
#include <memory>

class ParserFactory
{
public:
    std::unique_ptr<ParserInterface> create();
};

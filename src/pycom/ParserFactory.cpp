#include <pycom/factory/ParserFactory.hpp>
#include <pycom/parser/Parser.hpp>

std::unique_ptr<ParserInterface> ParserFactory::create()
{
    return std::make_unique<Parser>();
}
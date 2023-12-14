#include <pycom/utility/ErrorManager.hpp>
#include <cstdlib>

void ErrorManager::error(const char *_str, int _code)
{
    stream << _str << std::endl;
    exit(_code);
}

ErrorManager::ErrorManager(std::ostream &_stream) : stream(_stream) {}

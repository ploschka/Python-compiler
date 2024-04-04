#include <pycom/utility/ErrorManager.hpp>

void ErrorManager::error(const std::string &_str, void *_arg)
{
    stream << _str << std::endl;
    if (handler)
    {
        handler(_arg);
    }
}

void ErrorManager::error(const char *_str, void *_arg)
{
    stream << _str << std::endl;
    if (handler)
    {
        handler(_arg);
    }
}

void ErrorManager::registerHandler(error_handler_t _func)
{
    handler = _func;
}

ErrorManager::ErrorManager(std::ostream &_stream) : stream(_stream) {}

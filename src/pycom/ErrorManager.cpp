#include <pycom/utility/ErrorManager.hpp>

void ErrorManager::error(const std::string &_str, void *)
{
    stream << _str << std::endl;
    if (handler)
    {
        handler(nullptr);
    }
}

void ErrorManager::error(const char *_str, void *)
{
    stream << _str << std::endl;
    if (handler)
    {
        handler(nullptr);
    }
}

void ErrorManager::registerHandler(error_handler_t _func)
{
    handler = _func;
}

ErrorManager::ErrorManager(std::ostream &_stream) : stream(_stream) {}

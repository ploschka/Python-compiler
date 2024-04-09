#include <pycom/utility/ErrorManager.hpp>

void ErrorManager::error(const std::string &_str, void *_arg)
{
    stream << _str << std::endl;
    if (handler)
    {
        handler(_arg);
    }
    error_code = 1;
}

void ErrorManager::registerHandler(error_handler_t _func)
{
    handler = _func;
}

bool ErrorManager::errorOccured()
{
    return error_code != 0;
}

void ErrorManager::clear()
{
    error_code = 0;
}

ErrorManager::ErrorManager(std::ostream &_stream) : stream(_stream), error_code(0) {}

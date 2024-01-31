#include <pycom/error_manager/ThrowErrorManager.hpp>
#include <stdexcept>

void ThrowErrorManager::error(const std::string &_str, void *)
{
    throw std::runtime_error(_str);
}

void ThrowErrorManager::error(const char *_str, void *)
{
    throw std::runtime_error(_str);
}

#include <cstdio>

extern "C"
{
    void putf(float a)
    {
        printf("%f\n", a);
    }

    void __list_push_int(float)
    {

    }

    void __list_push_str(const char*)
    {
        
    }

    void __list_push_bool(int)
    {
        
    }

    void* __list_load(void*)
    {
        return NULL;
    }

    void __list_store(void*)
    {
        
    }

    void* __list_next(void*)
    {
        return NULL;
    }

    void* __list_create()
    {
        return NULL;
    }
}

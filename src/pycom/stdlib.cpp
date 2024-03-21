#include <cstdio>

extern "C"
{
    void putf(float a)
    {
        printf("%f\n", a);
    }
}

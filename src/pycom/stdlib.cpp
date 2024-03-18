#include <cstdio>

extern "C"
{

    void putf(float a)
    {
        printf("%f\n", a);
    }

    int __pymain();

    int main()
    {
        return __pymain();
    }
}

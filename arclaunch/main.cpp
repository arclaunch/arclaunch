#include <quiche.h>
#include <iostream>

#ifdef WIN32
#define AC_API __declspec(dllexport)
#else
#define AC_API
#endif

extern "C"
{

    AC_API const char *ver()
    {
        return quiche_version();
    };
}
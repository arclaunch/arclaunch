#include "main.hpp"

#include <boost/signals2.hpp>

typedef void (*cbChar)(const char *message);

// Signal type

boost::signals2::signal<void(const char *)> signal;

AC_API void RegisterCallback(cbChar callback)
{
    boost::signals2::connection conn = signal.connect(
        [callback](const char *msg)
        {
            callback(msg);
        });
};

AC_API void TriggerCallback()
{
    signal("Hello from C++!");
};
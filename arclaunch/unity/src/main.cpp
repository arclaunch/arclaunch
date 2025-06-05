#include "main.hpp"

#include <boost/signals2.hpp>

#include "jolt/provider.hpp"

typedef void (*cbChar)(const char *message);
typedef void (*cbPos)(const float x, const float y, const float z);

// Signal type

server::jolt::Provider *joltProvider;

boost::signals2::signal<void(const char *)>
    signal;

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

AC_API void SetupSim(cbPos callback)
{

    joltProvider = new server::jolt::Provider();
    joltProvider->posSignal.connect([callback](const JPH::Vec3 vec)
                                    { callback(vec.GetX(), vec.GetY(), vec.GetZ()); });
};

AC_API void RunSim()
{

    joltProvider->initial();
};
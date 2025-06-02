#include "jolt/bootstrap.hpp"

// always include Jolt.h first
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/Factory.h>

namespace server::jolt
{
    std::once_flag Bootstrap::flag;

    void Bootstrap::ensure()
    {
        std::call_once(flag, bootstrap);
    };

    void Bootstrap::bootstrap()
    {
        // Register allocation hook.
        // Must be done before any other jolt function is called
        // In this case we are using the default allocator (malloc/free)
        JPH::RegisterDefaultAllocator();

        // Install trace/assert callbacks

        // Create a "factory", responsible for creating instances of classes based on name/hash (eg for deserialization)
        // Required even if unused
        JPH::Factory::sInstance = new JPH::Factory();

        // Register all physics types with the factory and install their collision handlers
        // If we add custom types we should register their handlers before calling this function (using CollisionDispatch)
        JPH::RegisterTypes();
    }
};
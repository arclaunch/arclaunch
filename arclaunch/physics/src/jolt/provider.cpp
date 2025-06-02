#include "jolt/provider.hpp"

// Jolt includes
/// #include <Jolt/RegisterTypes.h>
// #include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include <thread>

#include "jolt/bootstrap.hpp"

namespace server::jolt
{
    Provider::Provider()
    {
        // Ensure that bootstrap func was called (once)
        Bootstrap::ensure();

        // Setup a temporary allocator for (temp) allocations during the physics update
        // Let's try assigning a block and falling back to malloc/free (slower) if the block is full.
        temp_allocator = new JPH::TempAllocatorImplWithMallocFallback(8 * 1024 * 1024); // 8 MB

        // We need a job system to run jobs on multiple theads
        // Using the example JobSystemThreadPool for now.
        job_system = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);
    };
};
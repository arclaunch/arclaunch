#ifndef JOLT_PROVIDER_H_
#define JOLT_PROVIDER_H_

// always include Jolt.h first
#include <Jolt/Jolt.h>

// Jolt includes
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "jolt/broad_phase.hpp"
#include "jolt/object.hpp"
#include "jolt/object_broad_phase_filter.hpp"

namespace server::jolt
{
    class Provider
    {
    private:
        /**
         * Max amount of rigidbodies that can be added to the physics system.
         * Trying to exceed this amount will cause an error.
         */
        const uint cMaxBodies = 65536;

        /**
         * How many mutexes to allocate to protect rigidbodies from concurrent access.
         * Set to zero to use default settings.
         */
        const uint cNumBodyMutexes = 0;

        /**
         * Max amount of body pairs that can be queued at any time.
         *
         * Note that the broad phase will detect overlapping body pairs
         * based on their bounding boxes and will insert them into a queue
         * for the narrowphase (from jolt helloworld)
         *
         * If this buffer is too small the queue will fill up and the broad
         * phase jobs will start to do narrow phase work (slightly less efficient)
         */
        const uint cMaxBodyPairs = 65536;

        /**
         * Max size of the constraint buffer.
         * If more contacts (collisions between bodies) are detected
         * then these additional contacts will be ignored and bodies will start
         * falling through the world.
         */
        const uint cMaxContactConstraints = 10240;

        /**
         * Temporaary allocator for (temp) allocations during the physics update
         */
        JPH::TempAllocatorImplWithMallocFallback *temp_allocator;

        /**
         * Job system, using Jolt-provided example for now.
         */
        JPH::JobSystemThreadPool *job_system;

        ::physics::jolt::broad_phase::LayerInterfaceImpl *broad_phase_layer_interface;
        ::physics::jolt::ObjectVsBroadPhaseLayerFilterImpl *object_vs_broad_phase_layer_filter;
        ::physics::jolt::object::ObjectLayerPairFilterImpl *object_vs_object_layer_filter;

        JPH::PhysicsSystem *physics_system;

    public:
        Provider();
        void initial();
        void start();
    };
};

#endif /* JOLT_PROVIDER_H_ */
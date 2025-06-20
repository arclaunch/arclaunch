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
#include "jolt/debug/recorder.hpp"

#include <boost/signals2.hpp>

namespace server::jolt
{
    class Instance
    {
    private:
        std::shared_ptr<JPH::PhysicsSystem> system;
        std::shared_ptr<::physics::jolt::debug::Recorder> recorder;

    public:
        static const std::string default_filename;

        Instance(std::shared_ptr<JPH::PhysicsSystem> sys, std::string filename = default_filename);

        inline std::shared_ptr<JPH::PhysicsSystem> getSystem() { return this->system; };
        inline std::shared_ptr<::physics::jolt::debug::Recorder> getRecorder() { return this->recorder; };
    };

    class Provider
    {
    private:
        /**
         * Max amount of rigidbodies that can be added to the physics system.
         * Trying to exceed this amount will cause an error.
         */
        const unsigned int cMaxBodies = 65536;

        /**
         * How many mutexes to allocate to protect rigidbodies from concurrent access.
         * Set to zero to use default settings.
         */
        const unsigned int cNumBodyMutexes = 0;

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
        const unsigned int cMaxBodyPairs = 65536;

        /**
         * Max size of the constraint buffer.
         * If more contacts (collisions between bodies) are detected
         * then these additional contacts will be ignored and bodies will start
         * falling through the world.
         */
        const unsigned int cMaxContactConstraints = 10240;

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

    public:
        Provider();

        void update(std::shared_ptr<Instance> inst, float delta_time, int steps);

        std::shared_ptr<Instance> create();

        std::vector<std::shared_ptr<Instance>> systems;
        ::physics::jolt::debug::Recorder *recorder;
    };
};

#endif /* JOLT_PROVIDER_H_ */
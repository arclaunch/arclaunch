#include "jolt/provider.hpp"

// Jolt includes
/// #include <Jolt/RegisterTypes.h>
// #include <Jolt/Core/Factory.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <thread>

#include "jolt/bootstrap.hpp"
#include "jolt/object.hpp"
#include <iostream>

#include <cmath>

// If you want your code to compile using single or double precision write 0.0_r to get a Real value that compiles to double or float depending if JPH_DOUBLE_PRECISION is set or not.
using namespace JPH::literals;

namespace server::jolt
{

    const std::string Instance::default_filename = std::string("state.jor");

    Instance::Instance(std::shared_ptr<JPH::PhysicsSystem> system, std::string filename)
    {
        this->system = system;
        this->recorder = std::make_shared<::physics::jolt::debug::Recorder>(filename.c_str());
    };

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

        // #region impl-specific

        // Create mapping table from object layer to broadphase layer
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        broad_phase_layer_interface = new ::physics::jolt::broad_phase::LayerInterfaceImpl();

        // Create class that filters object vs broadphase layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        object_vs_broad_phase_layer_filter = new ::physics::jolt::ObjectVsBroadPhaseLayerFilterImpl();

        // Create class that filters object vs object layers
        // Note: As this is an interface, PhysicsSystem will take a reference to this so this instance needs to stay alive!
        object_vs_object_layer_filter = new ::physics::jolt::object::ObjectLayerPairFilterImpl();

        // Now we can create the actual physics system. (call create)
    };

    void Provider::update(std::shared_ptr<Instance> inst, float delta_time, int steps)
    {

        inst->getSystem()->Update(delta_time, steps, temp_allocator, job_system);
        inst->getRecorder()->drawIfAvailable(inst->getSystem().get());
    };

    std::shared_ptr<Instance> Provider::create()
    {
        std::shared_ptr<JPH::PhysicsSystem> system = std::make_shared<JPH::PhysicsSystem>();
        system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broad_phase_layer_interface, *object_vs_broad_phase_layer_filter, *object_vs_object_layer_filter);

        std::shared_ptr<Instance> inst = std::make_shared<Instance>(system);
        systems.push_back(inst);
        return inst;
    }
};
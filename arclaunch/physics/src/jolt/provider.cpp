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

constexpr float operator""_uc(long double inValue) { return float(inValue * pow(10, -6)); };
constexpr float operator""_ukg(long double inValue) { return float(inValue * pow(10, -6)); };

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

        // Now we can create the actual physics system.
        physics_system = new JPH::PhysicsSystem();
        physics_system->Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *broad_phase_layer_interface, *object_vs_broad_phase_layer_filter, *object_vs_object_layer_filter);

        recorder = new ::physics::jolt::debug::Recorder("state.jor");
    };

    // demo from HelloWorld jolt
    void Provider::initial()
    {
        JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();

        // user selectable: electric field strength
        // placeholder
        float efs = 10.0f;

        float planeX = 3.0f;
        float planeY = 2.0f;
        float distBetween = 3.0f;
        JPH::BoxShapeSettings plane1(JPH::Vec3(planeX / 2, planeY / 2, 0.1f));
        plane1.SetEmbedded();
        JPH::ShapeSettings::ShapeResult plane1_sr = plane1.Create();

        JPH::ShapeRefC plane1_shape = plane1_sr.Get();

        // positive plate
        JPH::BodyCreationSettings plane1_settings(plane1_shape, JPH::RVec3(planeX / 2, planeY / 2, 0), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);
        JPH::BodyID plane1_body = body_interface.CreateAndAddBody(plane1_settings, JPH::EActivation::Activate);

        // negative plate
        JPH::BodyCreationSettings plane2_settings(plane1_shape, JPH::RVec3(planeX / 2, planeY / 2, distBetween), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);
        JPH::BodyID plane2_body = body_interface.CreateAndAddBody(plane2_settings, JPH::EActivation::Activate);

        /**
// create floor
JPH::BoxShapeSettings floor_shape_settings(JPH::Vec3(100.0f, 1.0f, 100.0f));
floor_shape_settings.SetEmbedded(); // A ref counted object on the stack (base class RefTarget) should be marked as such to prevent it from being freed when its reference count goes to 0.

// Create the shape
JPH::ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
JPH::ShapeRefC floor_shape = floor_shape_result.Get(); // We don't expect an error here, but you can check floor_shape_result for HasError() / GetError()

// Create the settings for the body itself. Note that here you can also set other properties like the restitution / friction.
JPH::BodyCreationSettings floor_settings(floor_shape, JPH::RVec3(0.0_r, -1.0_r, 0.0_r), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);

// Create the actual rigid body
JPH::Body *floor = body_interface.CreateBody(floor_settings); // Note that if we run out of bodies this can return nullptr

// Add it to the world
body_interface.AddBody(floor->GetID(), JPH::EActivation::DontActivate);

*/

        float sphereRadius = 0.05f;
        float offset = 0.3f;
        JPH::RVec3 sphereOrigin(offset, offset, distBetween / 2);
        JPH::BodyCreationSettings sphere_settings(new JPH::SphereShape(sphereRadius), sphereOrigin, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, ::physics::jolt::object::layers::NON_MOVING); // temp non moving so no collide

        // mass start
        // https://github.com/jrouwe/JoltPhysics/discussions/967#discussioncomment-8720550
        JPH::MassProperties msp;

        float mass_kg = 1.0_ukg;
        std::wcout << "MASS_KG:" << mass_kg << std::endl;
        msp.ScaleToMass(mass_kg); // actual mass in kg

        sphere_settings.mMassPropertiesOverride = msp;
        sphere_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia;
        // mass end

        JPH::Body *charge = body_interface.CreateBody(sphere_settings); // Note that if we run out of bodies this can return nullptr
        JPH::BodyID sphere_id = charge->GetID();

        // Add it to the world
        body_interface.AddBody(charge->GetID(), JPH::EActivation::DontActivate);
        // JPH::BodyID sphere_id = body_interface.CreateAndAddBody(sphere_settings, JPH::EActivation::DontActivate);

        float sphereCharge = -0.5_uc;

        // input: velo
        // only x component
        JPH::Vec3 velo(2.0f, 0.0f, 0.0f);
        body_interface.SetLinearVelocity(sphere_id, velo);
        body_interface.SetGravityFactor(sphere_id, 0.0f);

        charge->GetMotionProperties()->SetLinearDamping(0.0f);

        // TODO: if particle leaves box with both planes as walls, stop/restart sim
        // -> if particle reaches >sizeX eg stop sim
        // same for y later

        // Now you can interact with the dynamic body, in this case we're going to give it a velocity.
        // (note that if we had used CreateBody then we could have set the velocity straight on the body before adding it to the physics system)
        // body_interface.SetLinearVelocity(sphere_id, JPH::Vec3(0.0f, -5.0f, 0.0f));

        physics_system->OptimizeBroadPhase();

        // We simulate the physics world in discrete time steps. 60 Hz is a good rate to update the physics system.
        const float cDeltaTime = 1.0f / 60.0f;

        unsigned int step = 0;

        while (step < 300)
        {

            JPH::RVec3 position_pre = body_interface.GetCenterOfMassPosition(sphere_id);
            JPH::Vec3 velocity_pre = body_interface.GetLinearVelocity(sphere_id);

            // Next step
            ++step;

            // form start

            if (step > 1)
            {

                JPH::Vec3 force(0.0f, 0.0f, 0.0f);
                // to be amended to acceleration
                float forceZ = (sphereCharge * efs);
                std::wcout << "Applying force:" << forceZ << "\n";

                force.SetZ(forceZ);
                body_interface.AddForce(sphere_id, force, JPH::EActivation::Activate);
            }
            // form end

            /*
            // Output current position and velocity of the sphere
            JPH::RVec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
            JPH::Vec3 velocity = body_interface.GetLinearVelocity(sphere_id);
            std::wcout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;
            */
            // If you take larger steps than 1 / 60th of a second you need to do multiple collision steps in order to keep the simulation stable. Do 1 collision step per 1 / 60th of a second (round up).
            const int cCollisionSteps = 1;

            // Step the world
            physics_system->Update(cDeltaTime, cCollisionSteps, temp_allocator, job_system);

            recorder->drawIfAvailable(physics_system);

            JPH::RVec3 position = body_interface.GetCenterOfMassPosition(sphere_id);
            JPH::Vec3 velocity = body_interface.GetLinearVelocity(sphere_id);

            recorder->mDebugRenderer->DrawArrow(body_interface.GetCenterOfMassPosition(sphere_id), body_interface.GetCenterOfMassPosition(sphere_id) + velocity, JPH::Color::sRed, 0.01f);

            recorder->mDebugRenderer->DrawCoordinateSystem(body_interface.GetCenterOfMassTransform(sphere_id), 0.5f);

            std::wcout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;

            // posSignal(body_interface.GetCenterOfMassPosition(sphere_id));

            // std::wcout << "F: " << body_interface.GetFriction(sphere_id) << std::endl;

            std::wcout
                << "Pos delta: " << position.GetX() - position_pre.GetX() << " velo: " << velocity.GetX() - velocity_pre.GetX() << " step: " << step << std::endl;
        }

        // delete logic
        std::wcout << "hello\n";
    };
};
#include "simulation/plates.hpp"

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

#include <Jolt/Physics/Collision/CollisionCollectorImpl.h>
#include <Jolt/Physics/Collision/CollideShape.h>

#include <iostream> // std::wcout

#include "simulation/event/body.hpp"
#include "simulation/event/end.hpp"

using server::jolt::Provider;

namespace physics::simulation
{
    void PlatesSimulation::setup(BaseOptions *options)
    {
        this->options = static_cast<BaseOptions *>(options);

        JPH::BodyInterface &body_interface = GetBodyInterface();

        // Create boundary box
        JPH::BoxShapeSettings boundarySettings(JPH::Vec3(getOptions()->plate_size_x / 2, getOptions()->plate_size_y / 2, getOptions()->plate_distance_between / 2));
        boundarySettings.SetEmbedded();
        JPH::ShapeSettings::ShapeResult boundarySr = boundarySettings.Create();
        JPH::ShapeRefC boundaryShape = boundarySr.Get();

        JPH::BodyCreationSettings boundaryBodySettings(boundaryShape, JPH::RVec3(getOptions()->plate_size_x / 2, getOptions()->plate_size_y / 2, getOptions()->plate_distance_between / 2), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);
        boundaryBodySettings.mIsSensor = true;
        boundaryBody = body_interface.CreateBody(boundaryBodySettings);
        JPH::BodyID boundaryBodyId = boundaryBody->GetID();
        body_interface.AddBody(boundaryBodyId, JPH::EActivation::Activate);
        body_interface.SetUserData(boundaryBodyId, TYPE_BOUNDARY);

        float plateHalfThickness = getOptions()->plate_thickness / 2;

        // Create the shape for the plates
        JPH::BoxShapeSettings plane1(JPH::Vec3(getOptions()->plate_size_x / 2, getOptions()->plate_size_y / 2, plateHalfThickness));
        plane1.SetEmbedded();
        JPH::ShapeSettings::ShapeResult plane1_sr = plane1.Create();
        JPH::ShapeRefC plane1_shape = plane1_sr.Get();

        // Add positive plate (z = 0)
        JPH::BodyCreationSettings positive_plane_settings(plane1_shape, JPH::RVec3(getOptions()->plate_size_x / 2, getOptions()->plate_size_y / 2, -plateHalfThickness), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);
        positive_plane_body = body_interface.CreateAndAddBody(positive_plane_settings, JPH::EActivation::Activate);
        body_interface.SetUserData(positive_plane_body, TYPE_PLATE_POS);

        // Add negative plate (z = distance between)
        JPH::BodyCreationSettings negative_plane_settings(plane1_shape, JPH::RVec3(getOptions()->plate_size_x / 2, getOptions()->plate_size_y / 2, getOptions()->plate_distance_between + plateHalfThickness), JPH::Quat::sIdentity(), JPH::EMotionType::Static, ::physics::jolt::object::layers::NON_MOVING);
        negative_plane_body = body_interface.CreateAndAddBody(negative_plane_settings, JPH::EActivation::Activate);
        body_interface.SetUserData(negative_plane_body, TYPE_PLATE_NEG);

        // Sphere settings
        JPH::RVec3 charge_origin(getOptions()->charge_position_offset, getOptions()->charge_position_offset, getOptions()->plate_distance_between / 2);
        JPH::BodyCreationSettings charge_settings(new JPH::SphereShape(getOptions()->charge_radius), charge_origin, JPH::Quat::sIdentity(), JPH::EMotionType::Dynamic, ::physics::jolt::object::layers::MOVING); // temp non moving so no collide

        // Sphere: mass settings
        // https://github.com/jrouwe/JoltPhysics/discussions/967#discussioncomment-8720550
        JPH::MassProperties msp;
        msp.ScaleToMass(getOptions()->mass_kg);

        // Apply mass properties to charge settings
        charge_settings.mMassPropertiesOverride = msp;
        charge_settings.mOverrideMassProperties = JPH::EOverrideMassProperties::CalculateInertia; // use provided mass to calc inertia
        charge_settings.mMotionQuality = JPH::EMotionQuality::LinearCast;

        // Sphere: create the body (don't use createandadd so we get the body object)
        charge = body_interface.CreateBody(charge_settings); // Note that if we run out of bodies this can return nullptr
        JPH::BodyID charge_id = charge->GetID();
        charge->SetUserData(TYPE_CHARGE);
        body_interface.AddBody(charge_id, JPH::EActivation::Activate);

        // Disable linear damping for charge using it's Body object
        charge->GetMotionProperties()->SetLinearDamping(0.0f);

        // Set intiial velocity for the charge
        JPH::Vec3 charge_velocity(getOptions()->velocityX, 0.0f, 0.0f);
        charge->SetLinearVelocity(charge_velocity);

        // Disable gravity for the charge
        body_interface.SetGravityFactor(charge_id, 0.0f);

        // add event listyener

        // Finish setup - should be common;
        provider->physics_system->OptimizeBroadPhase();

        // step can start
        emitBodyEvents();
    };

    void PlatesSimulation::preStep(int step)
    {
        JPH::BodyInterface &body_interface = GetBodyInterface();
        const JPH::BodyID charge_id = charge->GetID();

        JPH::Vec3 force(0.0f, 0.0f, 0.0f);
        // to be amended to acceleration
        float forceZ = (getOptions()->sphere_charge * getOptions()->electric_field_strength);
        std::wcout << "Applying force:" << forceZ << "\n";

        force.SetZ(forceZ);
        body_interface.AddForce(charge_id, force, JPH::EActivation::Activate);
    };

    void PlatesSimulation::postStep(int step)
    {
        JPH::BodyInterface &body_interface = GetBodyInterface();
        JPH::BodyID charge_id = charge->GetID();

        JPH::RVec3 position = body_interface.GetCenterOfMassPosition(charge_id);
        JPH::Vec3 velocity = body_interface.GetLinearVelocity(charge_id);

#ifdef JPH_DEBUG_RENDERER
        provider->recorder->mDebugRenderer->DrawArrow(body_interface.GetCenterOfMassPosition(charge_id), body_interface.GetCenterOfMassPosition(charge_id) + velocity, JPH::Color::sRed, 0.01f);
        provider->recorder->mDebugRenderer->DrawCoordinateSystem(body_interface.GetCenterOfMassTransform(charge_id), 0.5f);
#endif

        std::wcout << "Step " << step << ": Position = (" << position.GetX() << ", " << position.GetY() << ", " << position.GetZ() << "), Velocity = (" << velocity.GetX() << ", " << velocity.GetY() << ", " << velocity.GetZ() << ")" << std::endl;

        // posSignal(body_interface.GetCenterOfMassPosition(charge_id));

        emitBodyEvents();

        ::JPH::AllHitCollisionCollector<::JPH::CollideShapeCollector> collector;
        provider->physics_system->GetNarrowPhaseQuery().CollideShape(
            charge->GetShape(),
            JPH::Vec3::sReplicate(1.0f),
            charge->GetCenterOfMassTransform(),
            JPH::CollideShapeSettings(),
            JPH::RVec3::sZero(),
            collector);

        bool isInBoundary = false;
        bool isContactingPlates = false;
        for (const auto result : collector.mHits)
        {

            if (!isInBoundary && result.mBodyID2 == boundaryBody->GetID())
                isInBoundary = true;

            if (!isContactingPlates && ((result.mBodyID2 == positive_plane_body) || (result.mBodyID2 == negative_plane_body)))
                isContactingPlates = true;
        };

        if (!isInBoundary || isContactingPlates)
        {
            std::wcout << "NOT IN BOUNDS\n";
            event::EndEvent *ev = new event::EndEvent(step, false);
            eventSignal(ev);
            exit(1);
        }
    };
}
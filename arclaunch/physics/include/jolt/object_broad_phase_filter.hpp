#ifndef JOLT_OBJECT_BROAD_PHASE_FILTER_HPP_
#define JOLT_OBJECT_BROAD_PHASE_FILTER_HPP_

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/ObjectLayer.h>

#include "jolt/broad_phase.hpp"
#include "jolt/object.hpp"

namespace physics::jolt
{
    class ObjectVsBroadPhaseLayerFilterImpl : public JPH::ObjectVsBroadPhaseLayerFilter
    {
    public:
        virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
        {
            switch (inLayer1)
            {
            case object::layers::NON_MOVING:
                return inLayer2 == broad_phase::layers::MOVING;
            case object::layers::MOVING:
                return true;
            default:
                JPH_ASSERT(false);
                return false;
            }
        }
    };
};

#endif /* JOLT_OBJECT_BROAD_PHASE_FILTER_HPP_ */
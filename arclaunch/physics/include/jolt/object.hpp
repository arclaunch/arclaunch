#ifndef JOLT_OBJECT_HPP_
#define JOLT_OBJECT_HPP_

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/ObjectLayer.h>

namespace physics::jolt
{
    namespace object
    {
        // Layer that objects can be in, determines which other objects it can collide with
        namespace layers
        {
            static constexpr ::JPH::ObjectLayer NON_MOVING = 0;
            static constexpr ::JPH::ObjectLayer MOVING = 1;
            static constexpr ::JPH::ObjectLayer NUM_LAYERS = 2;
        };

        // Class that determines if two object layers can collide
        class ObjectLayerPairFilterImpl : public JPH::ObjectLayerPairFilter
        {
            virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
            {
                switch (inObject1)
                {
                case layers::NON_MOVING:
                    return inObject2 == layers::MOVING; // Non moving only collides with moving
                case layers::MOVING:
                    return true; // Moving collides with everything
                default:
                    JPH_ASSERT(false);
                    return false;
                }
            }
        };
    };
};

#endif /* JOLT_OBJECT_HPP_ */
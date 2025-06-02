#include "jolt/broad_phase.hpp"

#include "jolt/object.hpp"

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace physics::jolt::broad_phase
{

    LayerInterfaceImpl::LayerInterfaceImpl()
    {
        // create a mapppping table from object to broad phase layer
        mObjectToBroadPhase[object::layers::NON_MOVING] = layers::NON_MOVING;
        mObjectToBroadPhase[object::layers::MOVING] = layers::MOVING;
    };
};
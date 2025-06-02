#ifndef JOLT_BROAD_PHASE_HPP_
#define JOLT_BROAD_PHASE_HPP_

#include <Jolt/Jolt.h>

#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>

namespace physics::jolt
{
    namespace broad_phase
    {
        // https://github.com/jrouwe/JoltPhysics/blob/80ea473407f997e2c667e97af2e12615bb898d81/HelloWorld/HelloWorld.cpp#L102
        namespace layers
        {
            static constexpr ::JPH::BroadPhaseLayer NON_MOVING(0);
            static constexpr ::JPH::BroadPhaseLayer MOVING(1);
            static constexpr uint NUM_LAYERS(2);
        };

        class LayerInterfaceImpl final : public ::JPH::BroadPhaseLayerInterface
        {
        public:
            LayerInterfaceImpl();

            virtual uint GetNumBroadPhaseLayers() const override
            {
                return layers::NUM_LAYERS;
            }

            virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
            {
                JPH_ASSERT(inLayer < layers::NUM_LAYERS);
                return mObjectToBroadPhase[inLayer];
            }

#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
            virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
            {
                switch ((JPH::BroadPhaseLayer::Type)inLayer)
                {
                case (JPH::BroadPhaseLayer::Type)broad_phase::layers::NON_MOVING:
                    return "NON_MOVING";
                case (JPH::BroadPhaseLayer::Type)broad_phase::layers::MOVING:
                    return "MOVING";
                default:
                    JPH_ASSERT(false);
                    return "INVALID";
                }
            }
#endif // JPH_EXTERNAL_PROFILE || JPH_PROFILE_ENABLED

        private:
            ::JPH::BroadPhaseLayer mObjectToBroadPhase[layers::NUM_LAYERS];
        };
    };
};

#endif /* JOLT_BROAD_PHASE_HPP_ */
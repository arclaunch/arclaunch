#ifndef EVENT_BODY_HPP_
#define EVENT_BODY_HPP_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "simulation/event/base.hpp"

namespace physics::simulation::event
{
    class BodyEvent : public BaseEvent
    {
    private:
        uint32_t id;
        uint32_t idIndex;
        uint32_t idSequenceNum;
        JPH::RVec3 position;
        JPH::RMat44 transform;

    public:
        BodyEvent(JPH::Body *body);

        inline uint32_t getId() { return this->id; };
        inline uint32_t getIdIndex() { return this->idIndex; };
        inline uint32_t getIdSequenceNum() { return this->idSequenceNum; };
        inline JPH::RVec3 getPosition() { return this->position; };
        inline JPH::RMat44 getTransform() { return this->transform; };
    };
}

#endif /* EVENT_BODY_HPP_ */
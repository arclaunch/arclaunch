#ifndef EVENT_END_HPP_
#define EVENT_END_HPP_

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>

#include "simulation/event/base.hpp"

namespace physics::simulation::event
{
    class EndEvent : public BaseEvent
    {
    private:
        unsigned int step;
        JPH::Vec3 point;
        bool cond;

    public:
        EndEvent(unsigned int step, JPH::Vec3 point, bool cond);

        inline unsigned int getStep() { return this->step; };
        inline JPH::Vec3 getPoint() { return this->point; };
        inline bool getCond() { return this->cond; };
    };
}

#endif /* EVENT_END_HPP_ */
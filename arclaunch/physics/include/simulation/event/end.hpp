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
        bool cond;

    public:
        EndEvent(unsigned int step, bool cond);

        inline unsigned int getSteo() { return this->step; };
        inline bool getCond() { return this->cond; };
    };
}

#endif /* EVENT_END_HPP_ */
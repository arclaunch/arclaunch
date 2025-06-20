#include "simulation/event/end.hpp"

namespace physics::simulation::event
{
    EndEvent::EndEvent(unsigned int step, JPH::RVec3 point, bool cond) : BaseEvent(EventType::END)
    {
        this->step = step;
        this->point = point;
        this->cond = cond;
    }
};
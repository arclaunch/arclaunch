#include "simulation/event/end.hpp"

namespace physics::simulation::event
{
    EndEvent::EndEvent(unsigned int step, bool cond) : BaseEvent(EventType::END)
    {
        this->step = step;
        this->cond = cond;
    }
};
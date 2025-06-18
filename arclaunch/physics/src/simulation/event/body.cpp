#include "simulation/event/body.hpp"

namespace physics::simulation::event
{
    BodyEvent::BodyEvent(JPH::Body *body) : BaseEvent(EventType::BODY)
    {
        JPH::BodyID bodyId = body->GetID();
        this->id = bodyId.GetIndexAndSequenceNumber();
        this->idIndex = bodyId.GetIndex();
        this->idSequenceNum = bodyId.GetSequenceNumber();
        this->position = body->GetCenterOfMassPosition();
        this->transform = body->GetCenterOfMassTransform();
        this->data = body->GetUserData();
    }
};
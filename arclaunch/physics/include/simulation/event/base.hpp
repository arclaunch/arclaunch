#ifndef EVENT_BASE_HPP_
#define EVENT_BASE_HPP_

namespace physics::simulation::event
{
    enum class EventType
    {
        UNKNOWN,
        BODY,
        END
    };

    class BaseEvent
    {
    private:
        EventType type;

    protected:
    public:
        inline EventType getType() const { return this->type; };
        BaseEvent(EventType type) { this->type = type; };
    };
}

#endif /* EVENT_BASE_HPP_ */
#ifndef SIMULATION_BASE_HPP_
#define SIMULATION_BASE_HPP_

#include <boost/signals2.hpp>

#include "jolt/provider.hpp"

#include "simulation/event/base.hpp"

namespace physics::simulation
{
    class DeltaTimeStep
    {
    private:
        float f;
        DeltaTimeStep(float step) : f(step) {};

    public:
        static const DeltaTimeStep FPS_60;

        operator float() const { return f; };
    };

    struct BaseOptions
    {
        int step_amount = 150;
        int step_collision_amount = 5; // 1 is too inaccurate for our purposes, further testing soon.
        float step_delta_time = DeltaTimeStep::FPS_60;
        bool debug_recorder_enabled = true;
    };

    class BaseSimulation
    {
    protected:
        unsigned int step = 0;

        BaseOptions *options;
        server::jolt::Provider *provider;
        inline JPH::BodyInterface &GetBodyInterface() { return provider->physics_system->GetBodyInterface(); };

        void emitBodyEvents();

    public:
        BaseSimulation(server::jolt::Provider *provider);
        virtual ~BaseSimulation() {};

        virtual void setup(BaseOptions *options) = 0;

        virtual void preStep(int step) = 0;
        virtual void postStep(int step) = 0;

        void execute();
        void tick();

        boost::signals2::signal<void(const physics::simulation::event::BaseEvent *)> eventSignal;
    };
}

#endif /* SIMULATION_BASE_HPP_ */
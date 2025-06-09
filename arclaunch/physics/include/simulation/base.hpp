#ifndef SIMULATION_BASE_HPP_
#define SIMULATION_BASE_HPP_

#include "jolt/provider.hpp"

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

    class BaseOptions
    {
    public:
        int step_amount = 300;
        int step_collision_amount = 1;
        float step_delta_time = DeltaTimeStep::FPS_60;
        bool debug_recorder_enabled = true;

        // We need this or any other virtual member to make Base polymorphic
        // (so we can use dynamic_cast or boost casting)
        // https://stackoverflow.com/a/36739219 (polymorphism)
        // https://www.boost.org/doc/libs/1_47_0/libs/conversion/cast.htm#Polymorphic_cast
        virtual ~BaseOptions() {}
    };

    class BaseSimulation
    {
    protected:
        BaseOptions *options;
        server::jolt::Provider *provider;
        inline JPH::BodyInterface &GetBodyInterface() { return provider->physics_system->GetBodyInterface(); };

    public:
        BaseSimulation(server::jolt::Provider *provider);
        virtual ~BaseSimulation() {};

        virtual void setup(BaseOptions *options) = 0;

        virtual void preStep(int step) = 0;
        virtual void postStep(int step) = 0;

        void execute();
    };
}

#endif /* SIMULATION_BASE_HPP_ */
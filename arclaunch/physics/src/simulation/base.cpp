#include "simulation/base.hpp"

using server::jolt::Provider;

namespace physics::simulation
{
    const DeltaTimeStep DeltaTimeStep::FPS_60(1.0f / 60.0f);

    BaseSimulation::BaseSimulation(Provider *provider)
    {
        this->provider = provider;
    };

    void BaseSimulation::execute()
    {
        unsigned int step = 0;
        while (step < options->step_amount)
        {
            ++step;

            preStep(step);

            provider->update(options->step_delta_time, options->step_collision_amount); // calls debug draw if avail

            postStep(step);
        };
    };
}
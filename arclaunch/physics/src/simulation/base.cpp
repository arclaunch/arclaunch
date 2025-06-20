#include "simulation/base.hpp"

#include "simulation/event/body.hpp"

using server::jolt::Provider;

namespace physics::simulation
{
    const DeltaTimeStep DeltaTimeStep::FPS_60(1.0f / 60.0f);
    const DeltaTimeStep DeltaTimeStep::FPS_120(1.0f / 120.0f);

    BaseSimulation::BaseSimulation(Provider *provider)
    {
        this->provider = provider;
        this->inst = provider->create();
    };

    void BaseSimulation::execute()
    {
        while (step < options->step_amount)
        {
            tick();
        };
    };

    void BaseSimulation::tick()
    {

        ++step;
        preStep(step);

        provider->update(inst, options->step_delta_time, options->step_collision_amount); // calls debug draw if avail

        postStep(step);
    };

    void BaseSimulation::emitBodyEvents()
    {
        //        JPH::BodyInterface &body_interface = GetBodyInterface();

        JPH::BodyIDVector bodyIds;
        inst->getSystem()->GetBodies(bodyIds);
        const JPH::BodyLockInterfaceLocking &lock_interface = inst->getSystem()->GetBodyLockInterface();

        for (const auto &id : bodyIds)
        {

            // get body obj
            JPH::Body *body = lock_interface.TryGetBody(id);

            if (body != nullptr)
            {
                event::BodyEvent *ev = new event::BodyEvent(body);
                eventSignal(ev);
            }
        }
    };
}
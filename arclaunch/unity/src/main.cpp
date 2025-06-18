#include "main.hpp"

#include <boost/signals2.hpp>

#include "jolt/provider.hpp"
#include "simulation/plates.hpp"

typedef void (*cbChar)(const char *message);

typedef void (*cbEvent)(const physics::simulation::event::BaseEvent *ev);

// Signal type

server::jolt::Provider *
    joltProvider;

boost::signals2::signal<void(const char *)>
    exampleSignal;

AC_API void RegisterCallback(cbChar callback)
{
    boost::signals2::connection conn = exampleSignal.connect(
        [callback](const char *msg)
        {
            callback(msg);
        });
};

AC_API void TriggerCallback()
{
    exampleSignal("Hello from C++!");
};

physics::simulation::PlatesSimulation *sim;

AC_API void SetupSim(physics::simulation::PlatesOptions *opt, cbEvent callback)
{

    joltProvider = new server::jolt::Provider();
    sim = new physics::simulation::PlatesSimulation(joltProvider);
    sim->eventSignal.connect([callback](const physics::simulation::event::BaseEvent *ev)
                             { callback(ev); });
    sim->setup((physics::simulation::BaseOptions *)opt);
};

AC_API void RunSim()
{

    sim->execute();
};

AC_API void TickSim()
{

    sim->tick();
};
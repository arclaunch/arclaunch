#include <iostream> // std::wcout
#include <event2/event.h>

#include "cli.hpp"
#include "quic/server.hpp"

#include "jolt/provider.hpp"
#include "simulation/plates.hpp"

int main(int argc, char **argv)
{
    // Create an args struct
    cli::args_t args;

    // Try to parse arguments, returning exit code from CLI11 if not success
    if (int code = cli::parse(args, argc, argv); code != EXIT_SUCCESS)
        return code;

    // At this point we have all required variables.

    std::wcout << "Cert file: " << args.cert_file.c_str() << std::endl;
    std::wcout << "Key file: " << args.key_file.c_str() << std::endl;

    // libevent: create event base
    struct event_base *base = event_base_new();

    // start QUIC
    quic::Server server(&args, base);

    server.configure();

    // starting libevent loop
    // std::wcout << "Server listening" << std::endl;
    // event_base_dispatch(base);

    // jolt time
    server::jolt::Provider *joltProvider = new server::jolt::Provider();
    physics::simulation::PlatesOptions *opt = new physics::simulation::PlatesOptions();
    physics::simulation::PlatesSimulation *sim = new physics::simulation::PlatesSimulation(joltProvider);

    std::wcout << "EFS:" << opt->electric_field_strength << std::endl;
    sim->setup((physics::simulation::BaseOptions *)opt);

    sim->execute();
    sim->eventSignal.connect([callback](const physics::simulation::event::BaseEvent ev)
                             { callback(ev); });

    std::wcout
        << "Shutting down" << std::endl;

    return EXIT_SUCCESS;
};
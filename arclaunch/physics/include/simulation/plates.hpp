#ifndef SIMULATION_PLATES_HPP_
#define SIMULATION_PLATES_HPP_

#include "simulation/base.hpp"
#include "units.hpp"

namespace physics::simulation
{
    struct PlatesOptions : BaseOptions
    {
        float electric_field_strength = 10.0f;
        float mass_kg = 1.0_ukg;
        float sphere_charge = 0.5_uc;
        float velocityX = 2.0f;

        JPH::RVec3 launch_coords = JPH::RVec3(0.3f, 1.0f, 4.0f);
        JPH::RVec3 target_coords = JPH::RVec3(0.6f, 1.0f, 4.06333f);

        // not expected to change but here for flexiblity
        float plate_size_x = 8.0f;
        float plate_size_y = 8.0f;
        float plate_distance_between = 8.0f;
        float plate_thickness = 0.1f;

        // time step 5 tested with 0.04 through 0.08 inclusive
        float charge_radius = 0.05f; // 7.95 @ 0.05f (passing test case);
        float target_radius = 0.20f;
    };

    class PlatesSimulation : public BaseSimulation
    {
    private:
        const static uint64_t TYPE_PLATE_POS = 1;
        const static uint64_t TYPE_PLATE_NEG = 2;
        const static uint64_t TYPE_CHARGE = 3;
        const static uint64_t TYPE_BOUNDARY = 4;
        const static uint64_t TYPE_TARGET = 5;

        JPH::Body *charge = nullptr;
        JPH::Body *target = nullptr;
        JPH::Body *boundaryBody = nullptr;

        JPH::BodyID positive_plane_body;
        JPH::BodyID negative_plane_body;

        inline PlatesOptions *
        getOptions()
        {
            return static_cast<PlatesOptions *>(this->options);
        };

    public:
        PlatesSimulation(server::jolt::Provider *provider) : BaseSimulation(provider) {};

        void setup(BaseOptions *options);

        void preStep(int step);
        void postStep(int step);
    };
}

#endif /* SIMULATION_PLATES_HPP_ */
#ifndef SIMULATION_PLATES_HPP_
#define SIMULATION_PLATES_HPP_

#include "simulation/base.hpp"
#include "units.hpp"

namespace physics::simulation
{
    class PlatesOptions : public BaseOptions
    {
    public:
        float electric_field_strength = 10.0f;
        float mass_kg = 1.0_ukg;
        float sphere_charge = 0.5_uc;
        float velocityX = 2.0f;

        // not expected to change but here for flexiblity
        float plate_size_x = 3.0f;
        float plate_size_y = 2.0f;
        float plate_distance_between = 3.0f;
        float plate_thickness = 0.1f;

        float charge_radius = 0.05f;
        float charge_position_offset = 0.3f;
    };
    class PlatesSimulation : public BaseSimulation
    {
    private:
        JPH::Body *charge = nullptr;

        inline PlatesOptions *
        getOptions()
        {
            return dynamic_cast<PlatesOptions *>(this->options);
        };

    public:
        PlatesSimulation(server::jolt::Provider *provider) : BaseSimulation(provider) {};

        void setup(BaseOptions *options);

        void preStep(int step);
        void postStep(int step);
    };
}

#endif /* SIMULATION_PLATES_HPP_ */
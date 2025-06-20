#include "gtest/gtest.h"

#include "plates_test.hpp"

#include <list>

std::list<plates_input> inputs;

float toleranceAbs = 0.085f;

void addInput(float electric_field_strength, float mass_kg, float sphere_charge, float velocityX, JPH::RVec3 launch_coords, JPH::RVec3 expected_position)
{

    physics::simulation::PlatesOptions *options = new physics::simulation::PlatesOptions();
    options->electric_field_strength = electric_field_strength;
    options->mass_kg = mass_kg;
    options->sphere_charge = sphere_charge;
    options->velocityX = velocityX;
    options->launch_coords = launch_coords;

    options->target_coords = JPH::RVec3(10, 10, 10); // out of bounds

    plates_input inp;
    inp.test_case_display_name = std::string("test_").append(std::to_string(inputs.size() + 1));
    inp.input = options;
    inp.expected = {expected_position};
    inputs.push_back(inp);
};

void reg()
{
    addInput(2.67f, 1.0_ukg, 1.0_uc, 2.668f, JPH::RVec3(3, 1, 5), JPH::RVec3(7, 1, 8));
    addInput(2.67f, 1.0_ukg, -1.0_uc, 2.0668f, JPH::RVec3(3, 1, 5), JPH::RVec3(7, 1, 0));
    addInput(1.8f, 2.0_ukg, -1.0_uc, 1.5492f, JPH::RVec3(2, 1, 3), JPH::RVec3(6, 1, 0));
    addInput(1.8f, 2.0_ukg, -15.0_uc, 6.0f, JPH::RVec3(2, 1, 3), JPH::RVec3(6, 1, 0));
    addInput(10.0f, 2.0_ukg, -15.0_uc, 14.14f, JPH::RVec3(2, 1, 3), JPH::RVec3(6, 1, 0));
    addInput(3.0f, 1.0_ukg, 1.0_uc, 3.0f, JPH::RVec3(1, 1, 2), JPH::RVec3(7, 1, 8));
    addInput(3.0f, 1.0_ukg, 1.0_uc, 0.5f, JPH::RVec3(1, 1, 2), JPH::RVec3(2, 1, 8));
    addInput(0.5f, 1.0_ukg, 1.0_uc, 1.0f, JPH::RVec3(0.5, 1, 4), JPH::RVec3(4.5, 1, 8));
    // addInput(0.1_uc, 1.0_ukg, 1.0_uc, 0.00115f, JPH::RVec3(1, 1, 2), JPH::RVec3(5, 1, 8));
};

INSTANTIATE_TEST_SUITE_P(
    Suite, // prefix
    PlatesTest,
    testing::ValuesIn(inputs),
    // set name based on value in param
    // https://github.com/google/googletest/blob/main/docs/advanced.md#specifying-names-for-value-parameterized-test-parameters
    [](const testing::TestParamInfo<PlatesTest::ParamType> &info)
    {
        return info.param.test_case_display_name;
    });

std::shared_ptr<server::jolt::Provider> PlatesTest::m_provider = NULL;
std::shared_ptr<physics::simulation::PlatesSimulation> PlatesTest::m_simulation = NULL;
physics::simulation::event::EndEvent *PlatesTest::m_endEvent = nullptr;

void PlatesTest::SetUp()
{
    plates_input input = GetParam();

    m_provider = std::make_shared<server::jolt::Provider>();
    m_simulation = std::make_shared<physics::simulation::PlatesSimulation>(m_provider.get());
    m_simulation->setup(input.input);
};

void PlatesTest::TearDown()
{
    m_provider.reset();
    m_simulation.reset();
    m_endEvent = nullptr;
};

void PlatesTest::m_HandleEvent(const physics::simulation::event::BaseEvent *ev)
{
    switch (ev->getType())
    {
    case physics::simulation::event::EventType::END:
        m_endEvent = (physics::simulation::event::EndEvent *)ev;
    }
};

TEST_P(PlatesTest, execute)
{

    plates_input input = GetParam();

    m_simulation->eventSignal.connect([this](const physics::simulation::event::BaseEvent *ev)
                                      { this->m_HandleEvent(ev); });

    while (m_endEvent == nullptr)
    {
        m_simulation->tick();
    };

    ASSERT_FALSE(m_endEvent == nullptr);

    JPH::RVec3 expected = input.expected.charge_position;
    JPH::RVec3 actual = m_endEvent->getPoint();
    ASSERT_NEAR(expected.GetX(), actual.GetX(), toleranceAbs);
    ASSERT_NEAR(expected.GetY(), actual.GetY(), toleranceAbs);
    ASSERT_NEAR(expected.GetZ(), actual.GetZ(), toleranceAbs);
}
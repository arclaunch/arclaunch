#ifndef SRC_PLATES_TEST_HPP_
#define SRC_PLATES_TEST_HPP_

#include "simulation/plates.hpp"
#include "simulation/event/end.hpp"

typedef struct
{
    JPH::RVec3 charge_position;
} plates_input_expected;

typedef struct
{
    std::string test_case_display_name;
    physics::simulation::PlatesOptions *input;
    plates_input_expected expected;
} plates_input;

extern std::list<plates_input> inputs;

class PlatesTest : public ::testing::TestWithParam<plates_input>
{

protected:
    // You can do set-up work for each test here.
    PlatesTest() = default;

    // You can do clean-up work that doesn't throw exceptions here.
    virtual ~PlatesTest() = default; // default impl

    // If the constructor and destructor are not enough for setting up
    // and cleaning up each test, you can define the following methods:

    // Code here will be called immediately after the constructor (right
    // before each test).
    void SetUp();

    // Code here will be called immediately after each test (right
    // before the destructor).
    void TearDown();

    // static void SetUpTestSuite();
    // static void TearDownTestSuite();

    static std::shared_ptr<server::jolt::Provider> m_provider;
    static std::shared_ptr<physics::simulation::PlatesSimulation> m_simulation;

    static physics::simulation::event::EndEvent *m_endEvent;

    void m_HandleEvent(const physics::simulation::event::BaseEvent *ev);
};

void reg();

#endif /* SRC_PLATES_TEST_HPP_ */
#include "jolt/debug/recorder.hpp"

#include <iostream> // std::wcout

using namespace std;
using namespace JPH;

namespace physics::jolt::debug
{
    Recorder::Recorder(const char *filename)
    {
#ifdef JPH_DEBUG_RENDERER
        mStream = new ofstream;
        mStream->open(filename, ofstream::out | ofstream::binary | ofstream::trunc);
        if (mStream->is_open())
        {
            mStreamWrapper = new StreamOutWrapper(*mStream);
            mDebugRenderer = new DebugRendererRecorder(*mStreamWrapper);
        }
        else
        {
            delete mStream;
            mStream = nullptr;
        }
#else
        std::wcout << "Recorder constructor called but JPH_DEBUG_RENDERER not set, performing no action.\n";
#endif
    };

    Recorder::~Recorder()
    {
#ifdef JPH_DEBUG_RENDERER
        delete mDebugRenderer;
        delete mStreamWrapper;
        delete mStream;
#endif
    };

    void Recorder::drawIfAvailable(PhysicsSystem *physics_system)
    {
#ifdef JPH_DEBUG_RENDERER
        if (mDebugRenderer != nullptr)
        {
            physics_system->DrawBodies(BodyManager::DrawSettings(), mDebugRenderer);
            physics_system->DrawConstraints(mDebugRenderer);
            mDebugRenderer->EndFrame();
        }
#endif
    };
};
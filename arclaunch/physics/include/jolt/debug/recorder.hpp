#ifndef DEBUG_RECORDER_HPP_
#define DEBUG_RECORDER_HPP_

#include <fstream> // std::ofstream

#include <Jolt/Jolt.h>

#include <Jolt/Physics/PhysicsSystem.h>

#ifdef JPH_DEBUG_RENDERER
#include <Jolt/Core/StreamWrapper.h>
#include <Jolt/Renderer/DebugRendererRecorder.h>
#endif

namespace physics::jolt::debug
{
    class Recorder
    {
    public:
        Recorder(const char *filename);
        ~Recorder();

        void drawIfAvailable(JPH::PhysicsSystem *physics_system);

#ifdef JPH_DEBUG_RENDERER
        JPH::DebugRendererRecorder *mDebugRenderer = nullptr;
        std::ofstream *mStream = nullptr;
        JPH::StreamOutWrapper *mStreamWrapper = nullptr;
#endif
    };
}

#endif /* DEBUG_RECORDER_HPP_ */
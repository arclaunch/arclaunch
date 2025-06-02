#ifndef JOLT_BOOTSTRAP_H_
#define JOLT_BOOTSTRAP_H_

#include <mutex>

namespace server::jolt
{
    class Bootstrap
    {
    private:
        static std::once_flag flag;
        static void bootstrap();

    public:
        static void ensure();
    };
};

#endif /* JOLT_BOOTSTRAP_H_ */
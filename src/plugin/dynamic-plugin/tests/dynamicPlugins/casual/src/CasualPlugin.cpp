#include "Engine.hpp"

#if defined(_WIN32)
#    define DYNAMIC_TEST_EXPORT extern "C" __declspec(dllexport)
#else
#    define DYNAMIC_TEST_EXPORT extern "C"
#endif

class Plugin : public Engine::APlugin {
  public:
    explicit Plugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~Plugin() = default;

    void Bind() final
    {
        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &) { Log::Info("Hello world!"); });
    }
};

DYNAMIC_TEST_EXPORT Engine::APlugin *CreatePlugin(Engine::Core &core) { return new Plugin(core); }

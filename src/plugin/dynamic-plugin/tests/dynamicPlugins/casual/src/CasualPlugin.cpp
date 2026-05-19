#include "Engine.hpp"

#if defined(_WIN32)
#    define DYNAMIC_TEST_EXPORT extern "C" __declspec(dllexport)
#else
#    define DYNAMIC_TEST_EXPORT extern "C"
#endif

class CasualPlugin : public Engine::APlugin {
  public:
    using Engine::APlugin::APlugin;
    ~CasualPlugin() = default;

    void Bind(void) override
    {
        RegisterSystems<Engine::Scheduler::Update>([](Engine::Core &) { Log::Info("Hello world!"); });
    }

  private:
    entt::hashed_string _dynamicLibraryId;
};

DYNAMIC_TEST_EXPORT Engine::APlugin *CreatePlugin(Engine::Core *core) { return new CasualPlugin(*core); }

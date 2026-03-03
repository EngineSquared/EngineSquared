#include "entity/Entity.hpp"
#include "plugin/APlugin.hpp"
#include <dlfcn.h>
#include <entity/EntityId.hpp>
#include <entt/entt.hpp>
#include <filesystem>
#include <gtest/gtest.h>

class MetaPlugin : public Engine::APlugin {
  public:
    explicit MetaPlugin(Engine::Core &core)
        : Engine::APlugin(core) {
              // empty
          };
    ~MetaPlugin() override = default;

    void Bind() final {}
};

struct Position {
    float x;
    float y;
};

struct Context {
    std::unordered_map<std::string, uint32_t> componentIds;
    Engine::Core *core = nullptr;
};

using ViewForEachCallback = void (*)(Engine::Id, void *userData);

using RuntimeView =
    entt::basic_runtime_view<entt::basic_sparse_set<Engine::Id>, std::allocator<entt::basic_sparse_set<Engine::Id> *>>;

struct MetaCore {
    std::unique_ptr<Context> context = nullptr;
    uint32_t (*GetComponentId)(Context *, const char *) = nullptr;
    RuntimeView *(*CreateView)(Context *) = nullptr;
    void (*DestroyView)(RuntimeView *) = nullptr;
    void (*AggregateComponentToView)(Context *, void *, uint32_t) = nullptr;
    void (*ViewForEach)(Context *, void *, void *, ViewForEachCallback) = nullptr;
    void *(*GetEntityComponentFromView)(Context *, uint32_t, Engine::Id) = nullptr;
    void (*AddSystem)(MetaCore *, MetaCore *(*) (MetaCore *) ) = nullptr;
};

TEST(Meta, smoke)
{
    Engine::Core core;

    auto entity1 = core.CreateEntity();
    auto entity2 = core.CreateEntity();

    entity1.AddComponent<Position>(69.0f, 42.0f);
    entity2.AddComponent<Position>(67.0f, 123456789.0f);

    std::filesystem::path currentPath = std::filesystem::current_path() / "src" / "plugin" / "zig" / "tests";
    std::filesystem::path pluginPath = currentPath / "zig_project" / "zig-out" / "lib" / "libtest.dylib";
    if (!std::filesystem::exists(pluginPath))
    {
        FAIL() << "Plugin not found at path: " << pluginPath;
    }
    void *handle = dlopen(pluginPath.c_str(), RTLD_LAZY);
    void *bind_symbole = dlsym(handle, "plugin_bind");
    if (bind_symbole == nullptr)
    {
        FAIL() << "Failed to load symbol: " << dlerror();
    }
    void *system = dlsym(handle, "system");
    if (system == nullptr)
    {
        FAIL() << "Failed to load symbol: " << dlerror();
    }
    entt::registry registry;
    entt::runtime_view view;
    entt::id_type uint32_t_type_id = entt::type_hash<uint32_t>::value();
    view.iterate(*registry.storage(uint32_t_type_id));

    typedef void (*BindFunction)(MetaCore *);
    typedef void (*SystemFunction)(void *);

    MetaCore metaCore;

    std::unordered_map<std::string, uint32_t> componentIds;
    componentIds["Position"] = entt::type_hash<Position>::value();

    metaCore.context = std::make_unique<Context>(componentIds, &core);
    metaCore.GetComponentId = [](Context *ctx, const char *name) -> uint32_t {
        if (ctx->componentIds.contains(name))
        {
            return ctx->componentIds[name];
        }
        return 0;
    };

    metaCore.CreateView = [](Context *ctx) -> RuntimeView * { return new RuntimeView(); };

    metaCore.DestroyView = [](RuntimeView *view) { delete view; };

    metaCore.AggregateComponentToView = [](Context *ctx, void *viewPtr, uint32_t componentId) {
        RuntimeView *runtimeView = static_cast<RuntimeView *>(viewPtr);
        const auto storage = ctx->core->GetRegistry().storage(componentId);
        if (storage == nullptr)
            return;
        runtimeView->iterate(*storage);
    };

    metaCore.ViewForEach = [](Context *ctx, void *viewPtr, void *userData, ViewForEachCallback callback) {
        RuntimeView *runtimeView = static_cast<RuntimeView *>(viewPtr);
        using CallbackType = void (*)(Engine::Id, void *userData);
        CallbackType callbackFunc = static_cast<CallbackType>(callback);
        if (callbackFunc == nullptr)
        {
            Log::Error("Callback function is null");
        }
        runtimeView->each([callbackFunc, userData](auto entity) { callbackFunc(entity, userData); });
    };

    metaCore.GetEntityComponentFromView = [](Context *ctx, uint32_t componentId, Engine::Id entityId) -> void * {
        auto component = ctx->core->GetRegistry().storage(componentId)->value(entityId);
        return component;
    };

    metaCore.AddSystem = [](MetaCore *core, MetaCore *(*system)(MetaCore *) ) {
        if (!core->context)
        {
            Log::Error("Context is null");
            return;
        }
        Context &context = *core->context;
        context.core->RegisterSystem([core, system = system](Engine::Core &) {
            if (!core->context)
            {
                Log::Error("Context is null");
                return;
            }
            Context &context = *core->context;
            system(core);
        });
    };

    BindFunction bind_function = reinterpret_cast<BindFunction>(bind_symbole);
    bind_function(&metaCore);

    // SystemFunction system_function = reinterpret_cast<SystemFunction>(system);
    // system_function(&metaCore);

    dlclose(handle);

    core.RunSystems();
}

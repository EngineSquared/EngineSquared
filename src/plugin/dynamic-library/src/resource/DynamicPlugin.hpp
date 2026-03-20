#pragma once

#include "plugin/APlugin.hpp"
#include "resource/ComponentsMeta.hpp"
#include "resource/Handle.hpp"
#include "resource/RuntimeView.hpp"
#include <any>
#include <unordered_map>

namespace DynamicLibrary::Resource {
class DynamicPlugin : public Engine::APlugin {
  private:
    struct CoreInterface {
        Engine::Core *core = nullptr;
        uint32_t (*constGetComponentId)(CoreInterface *, const char *) = [](CoreInterface *ctx,
                                                                            const char *name) -> uint32_t {
            return ctx->core->GetResource<DynamicLibrary::Resource::ComponentsMeta>()
                .GetComponent(name)
                .value_or(DynamicLibrary::Resource::ComponentsMeta::errorComponentMeta)
                .get()
                .id;
        };
        RuntimeView *(*constCreateView)(CoreInterface *) = [](CoreInterface *ctx) -> RuntimeView * {
            return new RuntimeView();
        };
        void (*constDestroyView)(RuntimeView *) = [](RuntimeView *view) { delete view; };
        void (*constAggregateComponentToView)(CoreInterface *, void *, uint32_t) =
            [](CoreInterface *coreInterface, void *viewPtr, uint32_t componentId) {
                RuntimeView *runtimeView = static_cast<RuntimeView *>(viewPtr);
                const auto storage = coreInterface->core->GetRegistry().storage(componentId);
                if (storage == nullptr)
                    return;
                runtimeView->iterate(*storage);
            };
        void (*constViewForEach)(CoreInterface *, void *, void *, void (*)(Engine::Id, void *userData)) =
            [](CoreInterface *ctx, void *viewPtr, void *userData, void (*callback)(Engine::Id, void *userData)) {
                RuntimeView *runtimeView = static_cast<RuntimeView *>(viewPtr);
                using CallbackType = void (*)(Engine::Id, void *userData);
                CallbackType callbackFunc = static_cast<CallbackType>(callback);
                if (callbackFunc == nullptr)
                {
                    Log::Error("Callback function is null");
                }
                runtimeView->each([callbackFunc, userData](auto entity) { callbackFunc(entity, userData); });
            };
        void *(*constGetEntityComponent)(CoreInterface *, uint32_t, Engine::Id) =
            [](CoreInterface *ctx, uint32_t componentId, Engine::Id entityId) -> void * {
            auto component = ctx->core->GetRegistry().storage(componentId)->value(entityId);
            return component;
        };
        void (*AddSystem)(CoreInterface *, void (*)(CoreInterface *)) = [](CoreInterface *coreInterface,
                                                                           void (*system)(CoreInterface *)) {
            if (!coreInterface)
            {
                Log::Error("Context is null");
                return;
            }
            if (!coreInterface->core)
            {
                Log::Error("Core is null in context");
                return;
            }
            if (!system)
            {
                Log::Warn("System function pointer is null");
                return;
            }
            coreInterface->core->RegisterSystem(
                [coreInterface = coreInterface, system = system](Engine::Core &) { system(coreInterface); });
        };
    } _coreInterface;

    using BindFunction = void(CoreInterface *);
    using BindFunctionPtr = void (*)(CoreInterface *);

  public:
    explicit DynamicPlugin(Engine::Core &core, const std::filesystem::path &dynamicLibraryPath)
        : Engine::APlugin(core), _handle(dynamicLibraryPath)
    {
        _coreInterface.core = &core;
        LoadSymbol<void(CoreInterface *)>("plugin_bind");
    }
    void Bind() override
    {
        try
        {
            auto bindFunction = std::any_cast<BindFunctionPtr>(_symbols["plugin_bind"]);
            bindFunction(&_coreInterface);
        }
        catch (const std::bad_any_cast &e)
        {
            Log::Error(fmt::format("Failed to cast bind function: {}", e.what()));
            return;
        }
    }

  private:
    template <typename TFunctionType> void LoadSymbol(const std::string &symbolName)
    {
        _symbols[symbolName] = _handle.GetFunction<TFunctionType>(symbolName);
    }

    Handle _handle;
    std::unordered_map<std::string, std::any> _symbols;
};
} // namespace DynamicLibrary::Resource

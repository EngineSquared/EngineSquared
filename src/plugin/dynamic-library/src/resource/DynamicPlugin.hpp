#pragma once

#include "plugin/APlugin.hpp"
#include "resource/ComponentsMeta.hpp"
#include "resource/Handle.hpp"
#include "resource/RuntimeView.hpp"

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

  public:
    explicit DynamicPlugin(Engine::Core &core, std::filesystem::path dynamicLibraryPath)
        : Engine::APlugin(core), _handle(dynamicLibraryPath)
    {
        _coreInterface.core = &core;
        LoadSymbol("plugin_bind");
    }
    void Bind() override
    {
        using BindFunction = void (*)(CoreInterface *);
        BindFunction bindFunction = reinterpret_cast<BindFunction>(_symbols["plugin_bind"]);
        if (!bindFunction)
        {
            Log::Error("Failed to load bind function from plugin");
            return;
        }
        bindFunction(&_coreInterface);
    }

  private:
    void LoadSymbol(const std::string &symbolName)
    {
        void *symbol = _handle.GetSymbol(symbolName);
        if (!symbol)
        {
            Log::Error(fmt::format("Failed to load symbol: {}", symbolName));
            throw std::runtime_error("Failed to load symbol");
        }
        _symbols[symbolName] = symbol;
    }

    Handle _handle;
    std::unordered_map<std::string, void *> _symbols;
};
} // namespace DynamicLibrary::Resource

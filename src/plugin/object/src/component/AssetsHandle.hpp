#pragma once

#include "AssetsManager.hpp"
#include "Core.hpp"

namespace ES::Plugin::Object::Component
{
    template <typename TAssetType>
    class AssetsHandle
    {
    public:
        std::string name;

        entt::hashed_string id;

        AssetsHandle() = default;
        explicit AssetsHandle(const std::string &name) : name(name), id(entt::hashed_string(name.c_str())) {}

        //TODO: remove this inline
        inline TAssetType &Get(ES::Engine::Core &core) {
            #ifdef ES_DEBUG
            if (!core.GetResource<ES::Plugin::Object::Resource::AssetsManager<TAssetType>>().Contains(id))
            {
                ES::Utils::Log::Error(fmt::format("Asset with id {} not found", id));
                throw std::out_of_range("Asset not found"); // TODO: use proper error
            }
            #endif
            return core.GetResource<ES::Plugin::Object::Resource::AssetsManager<TAssetType>>().Get(id);
        }

        inline bool Exists(ES::Engine::Core &core) {
            return core.GetResource<ES::Plugin::Object::Resource::AssetsManager<TAssetType>>().Contains(id);
        }
    };
}

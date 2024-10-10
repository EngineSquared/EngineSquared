#pragma once

#include <unordered_map>

#include "AssetID.hpp"

namespace ES::Plugin::Object::Resource {
    template <typename TAssetType>
    class AssetsManager
    {
    public:
        AssetsManager()
            : _assets(std::unordered_map<ES::Plugin::Object::Utils::AssetID, std::shared_ptr<TAssetType>>())
        { }

        ~AssetsManager() = default;

        ES::Plugin::Object::Utils::AssetID Add(TAssetType&& asset)
        {
            static ES::Plugin::Object::Utils::AssetID id = 0;
            id++;
            _assets[id] = std::make_shared<TAssetType>(std::move(asset));
            return id;
        }

        TAssetType& Get(ES::Plugin::Object::Utils::AssetID id) { return *_assets[id]; }

        void Remove(ES::Plugin::Object::Utils::AssetID id) { _assets.erase(id); }

        bool Contains(ES::Plugin::Object::Utils::AssetID id) { return _assets.find(id) != _assets.end(); }

    private:
        std::unordered_map<ES::Plugin::Object::Utils::AssetID, std::shared_ptr<TAssetType>> _assets;
    };
}

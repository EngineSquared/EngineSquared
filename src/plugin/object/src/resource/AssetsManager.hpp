#pragma once

#include <unordered_map>

#include "AssetID.hpp"

namespace ES::Plugin::Object::Resource {
/**
 * AssetsManager is a simple class that store assets. It provides methods to add, get, and remove assets.
 * It should be used to manage assets like textures, shaders, and meshes.
 */
template <typename TAssetType> class AssetsManager {
  public:
    AssetsManager() : _assets(std::unordered_map<ES::Plugin::Object::Utils::AssetID, std::shared_ptr<TAssetType>>()) {}

    ~AssetsManager() = default;

    /**
     * Adds an asset to the manager. It must be an rvalue.
     *
     * \param   asset   asset to be added
     * \return  id of the added asset
     */
    ES::Plugin::Object::Utils::AssetID Add(TAssetType &&asset)
    {
        static ES::Plugin::Object::Utils::AssetID id = 1;
        id++;
        _assets[id] = std::make_shared<TAssetType>(std::move(asset));
        return id;
    }

    /**
     * Get the reference to a stored asset.
     * If asset don't exist , throw std::out_of_range.
     *
     * \param   id   id of the asset to get
     * \return  the wanted asset
     */
    TAssetType &Get(ES::Plugin::Object::Utils::AssetID id) { return *_assets[id]; }

    /**
     * Delete an asset from the manager.
     *
     * \param   id  id of the asset to delete
     */
    void Remove(ES::Plugin::Object::Utils::AssetID id) { _assets.erase(id); }

    /**
     * Check whenever the asset with given id exists in the manager.
     *
     * \param   id  id of the asset
     * \return  true if the asset exists, false otherwise.
     */
    bool Contains(ES::Plugin::Object::Utils::AssetID id) { return _assets.find(id) != _assets.end(); }

  private:
    std::unordered_map<ES::Plugin::Object::Utils::AssetID, std::shared_ptr<TAssetType>> _assets;
};
} // namespace ES::Plugin::Object::Resource

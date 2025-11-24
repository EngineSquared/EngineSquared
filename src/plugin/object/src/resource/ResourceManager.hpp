#pragma once

#include "Logger.hpp"
#include "exception/ResourceManagerError.hpp"

#include <entt/core/hashed_string.hpp>
#include <entt/resource/cache.hpp>
#include <fmt/format.h>
#include <optional>

namespace Object::Resource {

/**
 * ResourceManager is a simple class that store resources. It provides methods to add, get, and remove resources.
 * It should be used to manage resources like textures, shaders, and meshes.
 */
template <typename ResourceType> class ResourceManager {
  private:
    /**
     * @brief ResourceLoader structure is used to load a resource from another resource or from arguments.
     *
     * @note This structure is used by the entt resource cache.
     */
    struct ResourceLoader final {
        using result_type = std::shared_ptr<ResourceType>;

        result_type operator()(const ResourceType &resource) const { return std::make_shared<ResourceType>(resource); }
        result_type operator()(ResourceType &&resource) const
        {
            return std::make_shared<ResourceType>(std::move(resource));
        }

        template <typename... Args> result_type operator()(Args &&...args) const
        {
            return std::make_shared<ResourceType>(std::forward<Args>(args)...);
        }
    };

  public:
    ResourceManager() = default;

    ~ResourceManager() = default;

    ResourceManager(const ResourceManager &) = delete;
    ResourceManager &operator=(const ResourceManager &) = delete;

    ResourceManager(ResourceManager &&) noexcept = default;
    ResourceManager &operator=(ResourceManager &&) noexcept = default;

    /**
     * @brief Adds a resource to the manager.
     *
     * @note If the resource already exists, it will be overwritten.
     *
     * @param id  id of the resource
     * @param args  arguments to create the resource
     * @return the added resource
     */
    template <typename... Args> entt::resource<ResourceType> Add(const entt::hashed_string &id, Args &&...args)
    {
        auto ret = cache.load(id, std::forward<Args>(args)...);

        if (!ret.second)
        {
            Log::Warn(fmt::format("Resource with id {} already exists. Overwriting.", id.data()));
            ret = cache.force_load(id, std::forward<Args>(args)...);
        }

        return ret.first->second;
    }

    /**
     * @brief Adds a resource to the manager.
     *
     * @note If the resource already exists, it will be overwritten.
     *
     * @param id  id of the resource
     * @param resource  resource to add
     * @return the added resource
     */
    entt::resource<ResourceType> Add(const entt::hashed_string &id, const ResourceType &resource)
    {
        auto ret = cache.load(id, resource);

        if (!ret.second)
        {
            Log::Warn(fmt::format("Resource with id {} already exists. Overwriting.", id.data()));
            ret = cache.force_load(id, resource);
        }

        return ret.first->second;
    }

    /**
     * @brief Adds a resource to the manager.
     *
     * @note If the resource already exists, it will be overwritten.
     *
     * @param id  id of the resource
     * @param resource  resource to add
     * @return the added resource
     */
    entt::resource<ResourceType> Add(const entt::hashed_string &id, ResourceType &&resource)
    {
        auto ret = cache.load(id, std::move(resource));

        if (!ret.second)
        {
            Log::Warn(fmt::format("Resource with id {} already exists. Overwriting.", id.data()));
            ret = cache.force_load(id, std::move(resource));
        }

        return ret.first->second;
    }

    /**
     * @brief Get the reference to a stored resource.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist.
     *
     * @param id   id of the resource to get
     * @return the wanted resource
     */
    [[nodiscard]] ResourceType &Get(const entt::hashed_string &id)
    {
        auto resource = cache[id];

        if (!resource)
            throw ResourceManagerError(fmt::format("Resource with id {} not found.", id.data()));

        return *resource;
    }

    /**
     * @brief Get the reference to a stored resource.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist.
     *
     * @param id   id of the resource to get
     * @return the wanted resource
     */
    [[nodiscard]] const ResourceType &Get(const entt::hashed_string &id) const
    {
        auto &resource = cache[id];

        if (!resource)
            throw ResourceManagerError(fmt::format("Resource with id {} not found.", id));

        return *resource;
    }

    /**
     * @brief Delete an resource from the manager.
     *
     * @param id  id of the resource to delete
     */
    void Remove(const entt::hashed_string &id) { cache.erase(id); }

    /**
     * @brief Check whenever the resource with given id exists in the manager.
     *
     * @param id  id of the resource
     * @return true if the resource exists, false otherwise.
     */
    [[nodiscard]] bool Contains(const entt::hashed_string &id) { return cache.contains(id); }

    /**
     * @brief Set the default resource that will be used as fallback.
     *
     * @note The resource with the given id must already exist in the manager.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist.
     *
     * @param id  id of the resource to use as default
     */
    void SetDefault(ResourceType &&resource)
    {
        defaultResource = std::move(resource);
    }

    template <typename... Args>
    void SetDefault(Args &&...args)
    {
        defaultResource = ResourceType(std::forward<Args>(args)...);
    }

    /**
     * @brief Get the reference to a stored resource, or the default resource if it doesn't exist.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist and no default is set.
     *
     * @param id  id of the resource to get
     * @return the wanted resource, or the default resource if the requested one doesn't exist
     */
    [[nodiscard]] ResourceType &GetOrDefault(const entt::hashed_string &id)
    {
        auto resource = cache[id];

        if (resource)
            return *resource;

        if (!defaultResource.has_value())
            throw ResourceManagerError(
                fmt::format("Resource with id {} not found and no default resource is set.", id.data()));

        return *defaultResource;
    }

    /**
     * @brief Get the reference to a stored resource, or the default resource if it doesn't exist.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist and no default is set.
     *
     * @param id  id of the resource to get
     * @return the wanted resource, or the default resource if the requested one doesn't exist
     */
    [[nodiscard]] const ResourceType &GetOrDefault(const entt::hashed_string &id) const
    {
        const auto &resource = cache[id];

        if (resource)
            return *resource;

        if (!defaultResource.has_value())
            throw ResourceManagerError(
                fmt::format("Resource with id {} not found and no default resource is set.", id.data()));

        return *defaultResource;
    }

    /**
     * @brief Check if a default resource has been set.
     *
     * @return true if a default resource is set, false otherwise
     */
    [[nodiscard]] bool HasDefault() const { return defaultResource.has_value(); }

  private:
    entt::resource_cache<ResourceType, ResourceLoader> cache{};
    std::optional<ResourceType> defaultResource = std::nullopt;
};

} // namespace Object::Resource

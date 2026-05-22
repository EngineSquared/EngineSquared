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
            Log::Warning(fmt::format("Resource with id {} already exists. Overwriting.", id.data()));
            ret = cache.force_load(id, std::forward<Args>(args)...);
        }

        return ret.first->second;
    }

    template <typename... Args> entt::resource<ResourceType> Add(std::string_view id, Args &&...args)
    {
        return Add(entt::hashed_string{id.data(), id.size()}, std::forward<Args>(args)...);
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

    [[nodiscard]] ResourceType &Get(std::string_view id) { return Get(entt::hashed_string{id.data(), id.size()}); }

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
        const auto &resource = cache[id];

        if (!resource)
            throw ResourceManagerError(
                fmt::format("Resource with id {} not found.", std::string_view(id.data(), id.size())));

        return *resource;
    }

    [[nodiscard]] const ResourceType &Get(std::string_view id) const
    {
        return Get(entt::hashed_string{id.data(), id.size()});
    }

    /**
     * @brief Delete an resource from the manager.
     *
     * @param id  id of the resource to delete
     */
    void Remove(const entt::hashed_string &id) { cache.erase(id); }

    void Remove(std::string_view id) { Remove(entt::hashed_string{id.data(), id.size()}); }

    /**
     * @brief Check whenever the resource with given id exists in the manager.
     *
     * @param id  id of the resource
     * @return true if the resource exists, false otherwise.
     */
    [[nodiscard]] bool Contains(const entt::hashed_string &id) const { return cache.contains(id); }

    [[nodiscard]] bool Contains(std::string_view id) const
    {
        return Contains(entt::hashed_string{id.data(), id.size()});
    }

    /**
     * @brief Set the default resource that will be used as fallback.
     *
     * @note The resource with the given id must already exist in the manager.
     *
     * @exception ResourceManagerError if the resource with given id doesn't exist.
     *
     * @param id  id of the resource to use as default
     */
    void SetDefault(ResourceType &&resource) { defaultResource = std::move(resource); }

    template <typename... Args> void SetDefault(Args &&...args)
    {
        defaultResource = ResourceType(std::forward<Args>(args)...);
    }

    /**
     * @brief Get the default resource.
     *
     * @exception ResourceManagerError if no default resource is set.
     *
     * @return the default resource
     */
    [[nodiscard]] ResourceType &GetDefault()
    {
        if (!defaultResource.has_value())
            throw ResourceManagerError("No default resource is set.");

        return *defaultResource;
    }

    /**
     * @brief Get the default resource.
     *
     * @exception ResourceManagerError if no default resource is set.
     *
     * @return the default resource
     */
    [[nodiscard]] const ResourceType &GetDefault() const
    {
        if (!defaultResource.has_value())
            throw ResourceManagerError("No default resource is set.");

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

    [[nodiscard]] ResourceType &GetOrDefault(std::string_view id)
    {
        return GetOrDefault(entt::hashed_string{id.data(), id.size()});
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

    [[nodiscard]] const ResourceType &GetOrDefault(std::string_view id) const
    {
        return GetOrDefault(entt::hashed_string{id.data(), id.size()});
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

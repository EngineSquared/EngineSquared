#pragma once

#include "resource/ARenderPass.hpp"
#include <list>
#include <memory>
#include <queue>
#include <string>
#include <unordered_map>
#include <unordered_set>

template <typename TRenderPass>
concept CRenderPass = std::is_base_of_v<Graphic::Resource::ARenderPass, TRenderPass>;

namespace Graphic::Resource {

class RenderGraph {
  private:
    using ID = entt::hashed_string;

    struct IDHash {
        std::size_t operator()(const ID &id) const noexcept { return std::hash<std::size_t>{}(id.value()); }
    };

  public:
    RenderGraph(void) = default;
    ~RenderGraph() = default;

    RenderGraph(const RenderGraph &) = delete;
    RenderGraph &operator=(const RenderGraph &) = delete;

    RenderGraph(RenderGraph &&) = default;
    RenderGraph &operator=(RenderGraph &&) = default;

    template <CRenderPass TRenderPass> void Add(std::string_view name, TRenderPass &&renderPass)
    {
        ID id = GetID(name);
        if (_renderPasses.contains(id))
        {
            Log::Warn(fmt::format("RenderGraph: Render pass with name '{}' already exists. Skipping addition.", name));
            return;
        }
        Log::Debug(fmt::format("RenderGraph: Added render pass '{}'.", name));
        _renderPasses[id] = std::make_shared<TRenderPass>(std::forward<TRenderPass>(renderPass));
        _orderedIDs.push_back(id);
    }

    void Remove(std::string_view name);
    void Execute(Engine::Core &core);
    bool Contains(std::string_view name) const;
    void SetDependency(std::string_view nameBefore, std::string_view nameAfter);

  private:
    static ID GetID(std::string_view name) { return entt::hashed_string(name.data(), name.size()); }

    void Update(void);
    void TopologicalSort(void);
    void ProcessDependencies(ID current, std::queue<ID> &queue, std::unordered_map<ID, size_t, IDHash> &inDegree) const;

    bool _dirty = false;
    std::unordered_map<ID, std::shared_ptr<ARenderPass>, IDHash> _renderPasses;
    std::unordered_map<ID, std::unordered_set<ID, IDHash>, IDHash> _dependencies;
    std::list<ID> _orderedIDs;
};

} // namespace Graphic::Resource

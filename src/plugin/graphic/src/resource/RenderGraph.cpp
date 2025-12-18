#include "resource/RenderGraph.hpp"
#include "exception/RenderPassSortError.hpp"
#include <map>
#include <queue>

namespace Graphic::Resource {
void RenderGraph::Remove(std::string_view name)
{
    ID id = GetID(name);
    if (!this->Contains(name))
    {
        Log::Warn(fmt::format("RenderGraph: Cannot remove render pass '{}', it does not exist.", name));
        return;
    }
    Log::Debug(fmt::format("RenderGraph: Removed render pass '{}'.", name));
    _renderPasses.erase(id);
    _orderedIDs.remove(id);
    _dependencies.erase(id);
    for (auto &dep : _dependencies)
    {
        dep.second.erase(id);
    }
    _dirty = true;
}
void RenderGraph::Execute(Engine::Core &core)
{
    Update();
    for (const auto &id : _orderedIDs)
    {
        if (auto it = _renderPasses.find(id); it != _renderPasses.end())
        {
            it->second->Execute(core);
        }
        else
        {
            throw Exception::RenderPassSortError(
                fmt::format("RenderGraph: Render pass with ID '{}' not found during execution.", id.value()));
        }
    }
}
bool RenderGraph::Contains(std::string_view name) const { return this->_renderPasses.contains(GetID(name)); }
void RenderGraph::SetDependency(std::string_view nameBefore, std::string_view nameAfter)
{
    ID idBefore = GetID(nameBefore);
    ID idAfter = GetID(nameAfter);
    bool errorFound = false;

    if (!this->Contains(nameBefore))
    {
        Log::Warn(
            fmt::format("RenderGraph: Cannot set dependency, render pass '{}' (Before) does not exist.", nameBefore));
        errorFound = true;
    }
    if (!this->Contains(nameAfter))
    {
        Log::Warn(
            fmt::format("RenderGraph: Cannot set dependency, render pass '{}' (After) does not exist.", nameAfter));
        errorFound = true;
    }

    if (errorFound)
    {
        return;
    }

    _dirty = true;
    _dependencies[idAfter].insert(idBefore);
}

void RenderGraph::Update(void)
{
    if (!_dirty)
        return;
    TopologicalSort();
    _dirty = false;
}
void RenderGraph::TopologicalSort(void)
{
    _orderedIDs.clear();

    std::unordered_map<ID, size_t, IDHash> inDegree;
    for (const auto &[id, _] : _renderPasses)
    {
        inDegree[id] = 0;
    }

    for (const auto &[after, befores] : _dependencies)
    {
        if (_renderPasses.find(after) != _renderPasses.end())
        {
            inDegree[after] += befores.size();
        }
        else
        {
            Log::Warn(fmt::format("RenderGraph: Dependency references non-existent render pass with ID '{}'. Skipping.",
                                  after.value()));
        }
    }

    std::queue<ID> queue;
    for (const auto &[id, degree] : inDegree)
    {
        if (degree == 0)
        {
            queue.push(id);
        }
    }

    while (!queue.empty())
    {
        auto current = queue.front();
        queue.pop();

        if (auto it = _renderPasses.find(current); it != _renderPasses.end())
        {
            _orderedIDs.push_back(it->first);
        }

        ProcessDependencies(current, queue, inDegree);
    }

    if (_orderedIDs.size() != _renderPasses.size())
    {
        throw Exception::RenderPassSortError("Cyclic dependency detected between render passes.");
    }
}
void RenderGraph::ProcessDependencies(ID current, std::queue<ID> &queue,
                                      std::unordered_map<ID, size_t, IDHash> &inDegree) const
{
    for (const auto &[after, befores] : _dependencies)
    {
        if (befores.contains(current))
        {
            --inDegree[after];
            if (inDegree[after] == 0)
            {
                queue.push(after);
            }
        }
    }
}
} // namespace Graphic::Resource

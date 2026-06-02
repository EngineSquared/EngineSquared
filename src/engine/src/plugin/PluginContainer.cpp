#include "Engine.pch.hpp"

#include "plugin/PluginContainer.hpp"

Engine::PluginError::PluginError(const std::string &message) : msg(fmt::format("Plugin error: {}", message)) {}

const char *Engine::PluginError::what() const throw() { return this->msg.c_str(); }

void Engine::PluginContainer::Delete(std::type_index id)
{
    if (this->_plugins.contains(id))
    {
        Log::Debug(fmt::format("Deleting scheduler: {}", id.name()));
        this->_plugins.erase(id);
        this->_dirty = true;
        if (this->_dependencies.contains(id))
        {
            this->_dependencies.erase(id);
        }
        for (auto &[after, befores] : this->_dependencies)
        {
            if (befores.contains(id))
            {
                befores.erase(id);
            }
        }
    }
    else
    {
        Log::Warning(fmt::format("Trying to delete Plugin but it does not exist: {}", id.name()));
    }
}

void Engine::PluginContainer::ProcessDependencies(std::type_index current, std::queue<std::type_index> &q,
                                                  std::map<std::type_index, size_t> &inDegree) const
{
    for (const auto &[after, befores] : _dependencies)
    {
        if (befores.contains(current))
        {
            --inDegree[after];
            if (inDegree[after] == 0)
            {
                q.push(after);
            }
        }
    }
}

void Engine::PluginContainer::TopologicalSort()
{
    _orderedPlugins.clear();

    std::map<std::type_index, size_t> inDegree;
    for (const auto &[type, _] : _plugins)
    {
        inDegree[type] = 0;
    }

    for (const auto &[after, befores] : _dependencies)
    {
        uint8_t validBefores = 0;
        for (const auto &before : befores)
        {
            if (_plugins.contains(before))
                validBefores += 1;
        }
        inDegree[after] += validBefores;
    }

    std::queue<std::type_index> q;
    for (const auto &[type, degree] : inDegree)
    {
        if (degree == 0)
        {
            q.push(type);
        }
    }

    while (!q.empty())
    {
        auto current = q.front();
        q.pop();

        if (auto it = _plugins.find(current); it != _plugins.end())
        {
            _orderedPlugins.push_back(it->second);
        }

        ProcessDependencies(current, q, inDegree);
    }

    if (_orderedPlugins.size() != _plugins.size())
    {
        throw PluginError("Cyclic dependency detected between schedulers.");
    }
}

void Engine::PluginContainer::Update()
{
    if (!_dirty)
        return;
    TopologicalSort();
    _dirty = false;
}

#pragma once

#include "plugin/IPlugin.hpp"

#include <list>
#include <map>
#include <memory>
#include <queue>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <unordered_set>

namespace Engine {
class Core;
class PluginError : public std::exception {
  public:
    explicit PluginError(const std::string &message);

    const char *what() const throw() override;

  private:
    std::string msg;
};

class PluginContainer {
  public:
    PluginContainer() = default;

    ~PluginContainer() = default;

    template <typename TPlugin, typename... Args> void Add(Core &core, Args &&...args);

    template <typename TPlugin> std::optional<std::shared_ptr<IPlugin>> Get();

    std::optional<std::shared_ptr<IPlugin>> Get(std::type_index id);

    std::list<std::shared_ptr<IPlugin>> &Get(void)
    {
        Update();
        return this->_orderedPlugins;
    }

    template <typename TPlugin> void Delete();

    void Delete(std::type_index id);

    template <typename TBefore, typename TAfter> void Before();

    template <typename TAfter, typename TBefore> void After();

    inline void Before(std::type_index before, std::type_index after)
    {
        _dirty = true;
        _dependencies[after].insert(before);
    }

    inline void After(std::type_index after, std::type_index before) { Before(before, after); }

    template <typename TBefore, typename TAfter> void RemoveDependencyBefore();

    template <typename TAfter, typename TBefore> void RemoveDependencyAfter();

    void RemoveDependencyBefore(std::type_index before, std::type_index after)
    {
        _dirty = true;
        auto it = _dependencies.find(after);
        if (it != _dependencies.end())
        {
            it->second.erase(before);
            if (it->second.empty())
                _dependencies.erase(it);
        }
        else
        {
            Log::Warning(fmt::format("Dependency not found: {} -> {}", before.name(), after.name()));
        }
    }

    void RemoveDependencyAfter(std::type_index after, std::type_index before) { RemoveDependencyBefore(before, after); }

    template <typename TPlugin> bool Contains() const;

    bool Contains(std::type_index id) const;

  private:
    void Update();

    void TopologicalSort();

    void ProcessDependencies(std::type_index current, std::queue<std::type_index> &q,
                             std::map<std::type_index, size_t> &inDegree) const;

  private:
    bool _dirty = false;

    std::map<std::type_index, std::shared_ptr<IPlugin>> _plugins;

    std::unordered_map<std::type_index, std::unordered_set<std::type_index>> _dependencies;

    std::list<std::shared_ptr<IPlugin>> _orderedPlugins;
};
} // namespace Engine

#include "plugin/PluginContainer.ipp"

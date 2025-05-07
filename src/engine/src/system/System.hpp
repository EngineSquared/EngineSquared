#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#include "CallableFunction.hpp"
#include "FunctionContainer.hpp"
#include "Logger.hpp"

namespace ES::Engine {
// Forward declaration of Core class.
class Core;

using SystemBase = ES::Utils::FunctionContainer::BaseFunction<void, Core &>;

template <typename TCallable> using System = ES::Utils::FunctionContainer::CallableFunction<TCallable, void, Core &>;


class SystemListIterator {
private:
    using BaseIterator = typename std::list<std::unique_ptr<SystemBase>>::const_iterator;
    BaseIterator _current;
    BaseIterator _end;
    const std::set<ES::Utils::FunctionContainer::FunctionID>& _exclude;

public:
    SystemListIterator(BaseIterator start, BaseIterator end, const std::set<ES::Utils::FunctionContainer::FunctionID>& exclude)
      : _current(start), _end(end), _exclude(exclude) {
        skip_excluded(); // Initialize to first valid element
    }

    void skip_excluded() {
        while (_current != _end && _exclude.contains((*_current)->GetID())) {
            ++_current;
        }
    }

    SystemListIterator& operator++() {
        ++_current;
        skip_excluded();
        return *this;
    }

    const std::unique_ptr<SystemBase>& operator*() const { return *_current; }
    const std::unique_ptr<SystemBase>* operator->() { return std::to_address(_current); }

    bool operator!=(const SystemListIterator& other) const { return _current != other._current; }
};


class SystemList
{
  private:
    const std::list<std::unique_ptr<SystemBase>> &main_list;
    const std::set<ES::Utils::FunctionContainer::FunctionID> &exclude_list;
    
  public:
    SystemList(const std::list<std::unique_ptr<SystemBase>> &main, const std::set<ES::Utils::FunctionContainer::FunctionID> &exclude)
      : main_list(main), exclude_list(exclude) {}

      SystemListIterator begin() {
      return SystemListIterator(main_list.begin(), main_list.end(), exclude_list);
    }

    SystemListIterator end() {
      return SystemListIterator(main_list.end(), main_list.end(), exclude_list);
    }
};

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer : public ES::Utils::FunctionContainer::FunctionContainer<void, Core &> {
  public:
    template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems) { return AddFunctions(systems...); }

    inline decltype(auto) GetSystems() { return SystemList(GetFunctions(), _disabledSystems); }

    inline bool DeleteSystem(ES::Utils::FunctionContainer::FunctionID id) { return DeleteFunction(id); }

    inline void Disable(const ES::Utils::FunctionContainer::FunctionID &id)
    {
      if (!Contains(id)) {
        ES::Utils::Log::Warn(fmt::format("System with ID {} not found", id));
      } else if (_disabledSystems.contains(id)) {
        ES::Utils::Log::Warn(fmt::format("System with ID {} is already disabled", id));
      } else {
        _disabledSystems.insert(id);
      }
    }

    inline void Enable(const ES::Utils::FunctionContainer::FunctionID &id)
    {
      if (!Contains(id)) {
        ES::Utils::Log::Warn(fmt::format("System with ID {} not found", id));
      } else if (!_disabledSystems.contains(id)) {
        ES::Utils::Log::Warn(fmt::format("System with ID {} is already enabled", id));
      } else {
        _disabledSystems.erase(id);
      }
    }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }

  private:
    std::set<ES::Utils::FunctionContainer::FunctionID> _disabledSystems;
};

} // namespace ES::Engine

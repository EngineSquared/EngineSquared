#pragma once

#include <algorithm>
#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

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
    const std::set<ES::Utils::FunctionContainer::FunctionID> &_exclude;

  public:
    SystemListIterator(BaseIterator start, BaseIterator end,
                       const std::set<ES::Utils::FunctionContainer::FunctionID> &exclude)
        : _current(start), _end(end), _exclude(exclude)
    {
        skip_excluded(); // Initialize to first valid element
    }

    void skip_excluded()
    {
        while (_current != _end && _exclude.contains((*_current)->GetID()))
        {
            ++_current;
        }
    }

    SystemListIterator &operator++()
    {
        ++_current;
        skip_excluded();
        return *this;
    }

    const std::unique_ptr<SystemBase> &operator*() const { return *_current; }
    const std::unique_ptr<SystemBase> *operator->() { return std::to_address(_current); }

    bool operator!=(const SystemListIterator &other) const { return _current != other._current; }
};

class SystemList {
  private:
    const std::list<std::unique_ptr<SystemBase>> &main_list;
    const std::set<ES::Utils::FunctionContainer::FunctionID> &exclude_list;

  public:
    SystemList(const std::list<std::unique_ptr<SystemBase>> &main,
               const std::set<ES::Utils::FunctionContainer::FunctionID> &exclude)
        : main_list(main), exclude_list(exclude)
    {
    }

    SystemListIterator begin() { return SystemListIterator(main_list.begin(), main_list.end(), exclude_list); }

    SystemListIterator end() { return SystemListIterator(main_list.end(), main_list.end(), exclude_list); }
};

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer : public ES::Utils::FunctionContainer::FunctionContainer<void, Core &> {
  public:
    template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems)
    {
        return AddFunctions(systems...);
    }

    inline decltype(auto) GetSystems() { return GetFunctions(); }

    inline decltype(auto) DeleteSystem(const ES::Utils::FunctionContainer::FunctionID &id) { return DeleteFunction(id); }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
};

} // namespace ES::Engine

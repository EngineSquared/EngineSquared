#include "SchedulerContainer.hpp"

void ES::Engine::SchedulerContainer::DeleteScheduler(std::type_index id)
{
    if (this->_schedulers.contains(id))
    {
        ES::Utils::Log::Debug(fmt::format("Deleting scheduler: {}", id.name()));
        this->_schedulers.erase(id);
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
        ES::Utils::Log::Warn(fmt::format("Trying to delete Scheduler but it does not exist: {}", id.name()));
    }
}

void ES::Engine::SchedulerContainer::ProcessDependencies(std::type_index current, std::queue<std::type_index> &q,
                                                         std::unordered_map<std::type_index, size_t> &inDegree) const
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

void ES::Engine::SchedulerContainer::TopologicalSort()
{
    _orderedSchedulers.clear();

    std::unordered_map<std::type_index, size_t> inDegree;
    for (const auto &[type, _] : _schedulers)
    {
        inDegree[type] = 0;
    }

    for (const auto &[after, befores] : _dependencies)
    {
        inDegree[after] += befores.size();
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

        if (auto it = _schedulers.find(current); it != _schedulers.end())
        {
            _orderedSchedulers.push_back(it->second);
        }

        ProcessDependencies(current, q, inDegree);
    }

    if (_orderedSchedulers.size() != _schedulers.size())
    {
        throw SchedulerError("Cyclic dependency detected between schedulers.");
    }
}

void ES::Engine::SchedulerContainer::Update()
{
    if (!_dirty)
        return;
    TopologicalSort();
    _dirty = false;
}

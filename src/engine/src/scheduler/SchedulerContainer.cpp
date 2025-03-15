#include "SchedulerContainer.hpp"

void ES::Engine::SchedulerContainer::DeleteScheduler(std::type_index id)
          {
              if (_idToIndex.contains(id))
              {
  #ifdef DEBUG
                  ES::Utils::Log::Info(fmt::format("Deleting scheduler: {}", id.name()));
  #endif
                  _orderedSchedulers.erase(_orderedSchedulers.begin() + _idToIndex[id]);
                  for (auto &[key, value] : _idToIndex)
                  {
                      if (value > _idToIndex[id])
                      {
                          value--;
                      }
                  }
                  _idToIndex.erase(id);
              }
              else
              {
                  ES::Utils::Log::Warn(fmt::format("Scheduler does not exist: {}", id.name()));
              }
          }
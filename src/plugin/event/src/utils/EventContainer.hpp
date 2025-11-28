#pragma once

#include <any>
#include <memory>

#include "FunctionContainer.hpp"
#include "core/Core.hpp"

namespace Event::Utils {

struct IEventContainer {
    virtual ~IEventContainer() = default;
    virtual void Trigger(Engine::Core &core, const std::any &event) = 0;
};

template <typename TEvent>
class EventContainer : public IEventContainer,
                       public FunctionUtils::FunctionContainer<void, Engine::Core &, const TEvent &> {
  public:
    void Trigger(Engine::Core &core, const std::any &event) override
    {
        const TEvent &typedEvent = std::any_cast<const TEvent &>(event);
        for (auto &callback : this->GetFunctions())
        {
            callback->Call(core, typedEvent);
        }
    }
};

} // namespace Event::Utils

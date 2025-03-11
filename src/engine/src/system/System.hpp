#pragma once

#include <functional>
#include <vector>
#include <memory>
#include <entt/entt.hpp>
#include <iostream>

namespace ES::Engine {
    class Core;

    class SystemBase {
        public:
            virtual ~SystemBase() = default;
            virtual void operator()(Core &core) const = 0;
    };

    template <typename TSystem>
    class System : public SystemBase {
      public:
        System(TSystem system) : SystemBase(), _system(system) {}
        ~System() = default;

        void operator()(Core &core) const { _system(core); }
      private:
        TSystem _system;   
    };
  
      class SystemContainer {
        public:
            SystemContainer() = default;
            ~SystemContainer() = default;
    
            template<typename... TSystem>
            void AddSystems(TSystem... systems)
            {
                (AddSystem(systems), ...);
            }

            std::vector<std::unique_ptr<SystemBase>> &GetSystems() { return _orderedSystems; }
        
          private:
            template<typename TCallable>
            void AddSystem(TCallable callable)
            {
              std::size_t id = 0;
              if constexpr (std::is_class_v<TCallable>) {
                id = entt::type_id<TCallable>().hash();;
              } else {
                id = std::hash<TCallable>{}(callable);
              }

              if (_idToIndex.find(id) != _idToIndex.end()) {
                // TODO: log error
                std::cout << "System already exists" << std::endl;
                return;
              }
              std::size_t index = _orderedSystems.size();
              auto system = std::make_unique<System<TCallable>>(callable);
              _orderedSystems.push_back(std::move(system));
              _idToIndex[id] = index;
            }
  
        private:
          std::unordered_map<entt::id_type, std::size_t> _idToIndex; // used to create unique id for each system
          std::vector<std::unique_ptr<SystemBase>> _orderedSystems;
      };
}
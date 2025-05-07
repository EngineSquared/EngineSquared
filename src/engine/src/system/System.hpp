#pragma once

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

// <<<<<<< HEAD
//     /**
//      * @brief Pure virtual function call operator to be implemented by derived systems.
//      * @param core Reference to the Core object.
//      */
//     virtual void operator()(Core &core) const = 0;

//     SystemBase &Disable()
//     {
//         enabled = false;
//         return *this;
//     }

//     SystemBase &Enable()
//     {
//         enabled = true;
//         return *this;
//     }

//     bool enabled = true; ///< Flag to enable or disable the system.
// };

// /**
//  * @brief Template class for systems, derived from SystemBase. Used to store any kind of callable system.
//  * @tparam TSystem Type of the system.
//  */
// template <typename TCallable> class System : public SystemBase {
//   public:
//     /**
//      * @brief Constructor for System.
//      * @param system The system object.
//      */
//     explicit System(TCallable system) : SystemBase(), _system(system) {}
//     ~System() final = default;

//     /**
//      * @brief Call operator to execute the system.
//      * @param core Reference to the Core.
//      */
//     void operator()(Core &core) const final { _system(core); }

//   private:
//     [[no_unique_address]] TCallable _system;
// };
// =======
template <typename TCallable> using System = ES::Utils::FunctionContainer::CallableFunction<TCallable, void, Core &>;
// >>>>>>> main

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer : public ES::Utils::FunctionContainer::FunctionContainer<void, Core &> {
  public:
    template <typename... TSystem> inline void AddSystems(TSystem... systems) { AddFunctions(systems...); }

    inline const std::list<std::unique_ptr<SystemBase>> &GetSystems() { return GetFunctions(); }

// <<<<<<< HEAD
//     /**
//      * @brief Adds one or multiple systems to the container.
//      * @tparam TSystem Variadic template parameter for system types.
//      * @param systems The systems to be added.
//      */
//     template <typename... TSystem> inline decltype(auto) AddSystems(TSystem... systems)
//     {
//         // This immediat call ensures that the systems are added in the order they are passed.
//         // return [&]<std::size_t... I>(std::index_sequence<I...>)
//         // {
//         //     // Capture results in an array to enforce order
//         //     std::array<std::shared_ptr<SystemBase>, sizeof...(TSystem)> temp{
//         //         AddSystem(systems)...
//         //     };
//         //     return std::tuple{temp[I]...};
//         // }(std::make_index_sequence<sizeof...(TSystem)>{});

//         // This is a workaround to ensure that the systems are added in the order they are passed.
//         std::array<std::shared_ptr<SystemBase>, sizeof...(TSystem)> temp{AddSystem(systems)...};
//         return std::tuple_cat(temp);
//     }

//     /**
//      * @brief Retrieves the vector of systems.
//      * @return Reference to the vector of unique pointers to SystemBase.
//      */
//     inline std::list<std::shared_ptr<SystemBase>> &GetSystems() { return _orderedSystems; }

//   private:
//     /**
//      * @brief Adds a single system to the container.
//      * @tparam TCallable Type of the callable system.
//      * @param callable The callable system to be added.
//      */
//     template <typename TSystem> std::shared_ptr<SystemBase> AddSystem(TSystem callable);

//     std::unordered_map<entt::id_type, std::size_t> _idToIndex; ///< Map to store unique ids for each system.
//     std::list<std::shared_ptr<SystemBase>> _orderedSystems;    ///< Vector to store systems in order.
// =======
    inline bool DeleteSystem(ES::Utils::FunctionContainer::FunctionID id) { return DeleteFunction(id); }

  private:
    template <typename TCallable> void AddSystem(TCallable callable) { AddFunction(callable); }
// >>>>>>> main
};

} // namespace ES::Engine

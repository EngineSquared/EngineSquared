#pragma once

#include <entt/entt.hpp>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "Logger.hpp"

namespace ES::Engine {
// Forward declaration of Core class.
class Core;

/**
 * @brief Base class for all systems in the engine.
 */
class SystemBase {
  public:
    virtual ~SystemBase() = default;

    /**
     * @brief Pure virtual function call operator to be implemented by derived systems.
     * @param core Reference to the Core object.
     */
    virtual void operator()(Core &core) const = 0;
};

/**
 * @brief Template class for systems, derived from SystemBase. Used to store any kind of callable system.
 * @tparam TSystem Type of the system.
 */
template <typename TCallable> class System : public SystemBase {
  public:
    /**
     * @brief Constructor for System.
     * @param system The system object.
     */
    explicit System(TCallable system) : SystemBase(), _system(system) {}
    ~System() final = default;

    /**
     * @brief Call operator to execute the system.
     * @param core Reference to the Core.
     */
    void operator()(Core &core) const final { _system(core); }

  private:
    [[no_unique_address]] TCallable _system;
};

/**
 * @brief Container class for managing multiple systems.
 */
class SystemContainer {
  public:
    /**
     * @brief Default constructor for SystemContainer.
     */
    SystemContainer() = default;

    /**
     * @brief Default destructor for SystemContainer.
     */
    ~SystemContainer() = default;

    /**
     * @brief Adds one or multiple systems to the container.
     * @tparam TSystem Variadic template parameter for system types.
     * @param systems The systems to be added.
     */
    template <typename... TSystem> inline void AddSystems(TSystem... systems) { (AddSystem(systems), ...); }

    /**
     * @brief Retrieves the vector of systems.
     * @return Reference to the vector of unique pointers to SystemBase.
     */
    inline std::vector<std::unique_ptr<SystemBase>> &GetSystems() { return _orderedSystems; }

  private:
    /**
     * @brief Adds a single system to the container.
     * @tparam TCallable Type of the callable system.
     * @param callable The callable system to be added.
     */
    template <typename TSystem> void AddSystem(TSystem callable);

    std::unordered_map<entt::id_type, std::size_t> _idToIndex; ///< Map to store unique ids for each system.
    std::vector<std::unique_ptr<SystemBase>> _orderedSystems;  ///< Vector to store systems in order.
};

} // namespace ES::Engine

#include "System.inl"

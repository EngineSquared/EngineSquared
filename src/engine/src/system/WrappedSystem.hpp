#pragma once

#include "BaseFunction.hpp"
#include "Demangle.hpp"

namespace Engine {
class Core;

/// @class WrappedSystem
/// @brief Wrapper around a system that allows to add an error callback to it.
/// @tparam TSystem The type of the system to wrap.
/// @tparam TErrorCallback The type of the error callback to wrap.
template <typename TSystem, typename TErrorCallback>
class WrappedSystem : public FunctionUtils::BaseFunction<void, Core &> {
  public:
    /// @brief Constructor for WrappedSystem.
    /// @param system The system to be wrapped.
    /// @param errorCallback The error callback to be called if the system throws an exception.
    explicit WrappedSystem(TSystem system, TErrorCallback errorCallback);

    /// @brief Destructor for WrappedSystem.
    ~WrappedSystem() override = default;

    /// @brief Calls the system.
    /// @param core The core to pass to the system.
    void operator()(Engine::Core &core) const override;

    /// @brief Returns the unique ID of the system.
    /// @return Unique ID of the system.
    FunctionUtils::FunctionID GetID() const override;

    /// @brief Get the name of the system.
    /// @return Name of the system.
    std::string GetName() const override;

    /// @brief Get the ID of the system.
    /// @param callable The system.
    /// @tparam TSystem The type of the system.
    /// @return The ID of the system.
    static FunctionUtils::FunctionID GetCallableID(const TSystem &callable);

    /// @brief Get the name of the system.
    /// @param callable The system.
    /// @tparam TSystem The type of the system.
    /// @return The name of the system.
    static std::string GetCallableName(const TSystem &callable);

  private:
    /// @brief The wrapped system.
    [[no_unique_address]] TSystem _system;

    /// @brief The error callback to be called if the system throws an exception.
    [[no_unique_address]] TErrorCallback _errorCallback;

    /// @brief Unique ID for the system.
    FunctionUtils::FunctionID _id = 0;

    /// @brief Name of the system.
    std::string _name;
};
} // namespace Engine

#include "system/WrappedSystem.ipp"

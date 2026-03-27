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
    /// @todo Put the implementation in the cpp file
    explicit WrappedSystem(TSystem system, TErrorCallback errorCallback)
        : _system(system), _errorCallback(errorCallback)
    {
        _id = GetCallableID(_system);
        _name = GetCallableName(_system);
    }

    /// @brief Destructor for WrappedSystem.
    ~WrappedSystem() override = default;

    /// @brief Calls the system.
    /// @param core The core to pass to the system.
    /// @todo Put the implementation in the cpp file
    void operator()(Engine::Core &core) const override
    {
        try
        {
            return _system(core);
        }
        catch (const std::exception &) // NOSONAR
        {
            _errorCallback(core);
            throw;
        }
    }

    /// @brief Returns the unique ID of the system.
    /// @return Unique ID of the system.
    /// @todo Put the implementation in the cpp file
    FunctionUtils::FunctionID GetID() const override { return _id; }

    /// @brief Get the name of the system.
    /// @return Name of the system.
    /// @todo Put the implementation in the cpp file
    std::string GetName() const override { return _name; }

    /// @brief Get the ID of the system.
    /// @param callable The system.
    /// @tparam TSystem The type of the system.
    /// @return The ID of the system.
    /// @todo Put the implementation in the cpp file
    static FunctionUtils::FunctionID GetCallableID(const TSystem &callable)
    {
        if constexpr (std::is_class_v<TSystem>)
        {
            return typeid(callable).hash_code();
        }
        else
        {
            return std::hash<TSystem>{}(callable);
        }
    }

    /// @brief Get the name of the system.
    /// @param callable The system.
    /// @tparam TSystem The type of the system.
    /// @return The name of the system.
    /// @todo Put the implementation in the cpp file
    static std::string GetCallableName(const TSystem &callable)
    {
        if constexpr (std::is_class_v<TSystem>)
        {
            return FunctionUtils::DemangleTypeName(typeid(callable));
        }
        else
        {
            return std::to_string(GetCallableID(callable));
        }
    }

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

#pragma once

#include "BaseFunction.hpp"

namespace Engine {
// Forward declaration of Core class.
class Core;

/**
 * @brief Wrapper around a system that allows to add an error callback to it.
 */
template <typename TSystem, typename TErrorCallback>
class WrappedSystem : public FunctionUtils::BaseFunction<void, Core &> {
  public:
    /**
     * @brief Constructor for WrappedSystem.
     * @param callable The callable object to be stored.
     * @param errorCallback The error callback to be stored.
     */
    explicit WrappedSystem(TSystem system, TErrorCallback errorCallback)
        : _system(system), _errorCallback(errorCallback)
    {
        _id = GetCallableID(_system);
    }

    /**
     * @brief Destructor for WrappedSystem.
     */
    ~WrappedSystem() override = default;

    /**
     * @brief Calls the system.
     * @param args Arguments to pass to the system.
     * @return Return value of the system.
     */
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

    /**
     * @brief Returns the unique ID of the system.
     * @return Unique ID of the system.
     */
    FunctionUtils::FunctionID GetID() const override { return _id; }

    /**
     * @brief Get the ID of the system.
     * @param callable The system.
     * @tparam TCallable The type of the system.
     * @return The ID of the system.
     */
    static FunctionUtils::FunctionID GetCallableID(TSystem callable)
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

  private:
    [[no_unique_address]] TSystem _system;
    [[no_unique_address]] TErrorCallback _errorCallback;
    FunctionUtils::FunctionID _id = 0; ///< Unique ID for the function.
};
} // namespace Engine

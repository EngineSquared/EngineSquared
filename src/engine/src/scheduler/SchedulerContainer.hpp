#pragma once

#include "AScheduler.hpp"
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <unordered_map>
#include <vector>

namespace ES::Engine {
class Core;

class SchedulerError : public std::exception {
  public:
    explicit SchedulerError(const std::string &message) : msg(fmt::format("Scheduler error: {}", message)){};

    const char *what() const throw() override { return this->msg.c_str(); };

  private:
    std::string msg;
};

/**
 * @class SchedulerContainer
 * @brief Manages a collection of schedulers, allowing addition, retrieval, and deletion of schedulers.
 *
 * The SchedulerContainer class provides functionality to add, retrieve, and delete schedulers.
 * It maintains an ordered list of schedulers and a mapping from type_index to their position in the list.
 *
 */
class SchedulerContainer {
  public:
    using SchedulerStorage = std::vector<std::shared_ptr<Scheduler::AScheduler>>;

  public:
    SchedulerContainer() = default;
    ~SchedulerContainer() = default;

    /**
     * @brief Adds a scheduler to the container.
     *
     * This function creates a new scheduler of type TScheduler with the provided arguments
     * and adds it to the container. If a scheduler of the same type already exists, a warning
     * is logged and the function returns without adding a new scheduler.
     *
     * @tparam TScheduler The type of the scheduler to be added.
     * @tparam Args The types of the arguments to be forwarded to the scheduler's constructor.
     * @param core A reference to the Core object required by the scheduler's constructor.
     * @param args The arguments to be forwarded to the scheduler's constructor.
     */
    template <typename TScheduler, typename... Args> void AddScheduler(Core &core, Args &&...args);

    /**
     * @brief Retrieves a scheduler of the specified type.
     *
     * This function searches for a scheduler of the specified type TScheduler
     * within the container. If the scheduler is found, it returns a reference
     * to it. If the scheduler is not found, it throws an error.
     *
     * @tparam TScheduler The type of the scheduler to retrieve.
     * @return TScheduler& A reference to the scheduler of the specified type.
     * @throws SchedulerError If the scheduler of the specified type is not found.
     */
    template <typename TScheduler> TScheduler &GetScheduler();

    /**
     * @brief Retrieves a scheduler identified by the given type index.
     *
     * This function searches for a scheduler associated with the specified type index.
     * If the scheduler is found, it returns a weak pointer to it. If not found, it throws an error.
     *
     * @param id The type index of the scheduler to retrieve.
     * @return std::weak_ptr<Scheduler::AScheduler> A weak pointer to the scheduler of the specified type.
     * @throws SchedulerError If the scheduler of the specified type is not found.
     */
    std::shared_ptr<Scheduler::AScheduler> GetScheduler(std::type_index id);

    /**
     * @brief Retrieves the list of schedulers.
     *
     * This function returns a reference to a vector containing unique pointers
     * to AScheduler objects. The vector maintains the order of the schedulers.
     *
     * @return A reference to a vector of unique pointers to AScheduler objects.
     */
    inline SchedulerStorage &GetSchedulers() { return _orderedSchedulers; }

    /**
     * @brief Deletes a scheduler of the specified type.
     *
     * This function deletes a scheduler instance identified by the type provided
     * as the template parameter. It internally calls the DeleteScheduler function
     * with the type index of the specified scheduler type.
     *
     * @tparam TScheduler The type of the scheduler to be deleted.
     */
    template <typename TScheduler> void DeleteScheduler() { DeleteScheduler(std::type_index(typeid(TScheduler))); }

    /**
     * @brief Deletes a scheduler identified by the given type index.
     *
     * This function removes the scheduler associated with the specified type index from the container.
     * If the scheduler exists, it will be erased from the ordered list of schedulers and the internal
     * mapping of type indices to their respective positions will be updated accordingly.
     *
     * @param id The type index of the scheduler to be deleted.
     *
     * @note If the scheduler does not exist, a warning message will be logged.
     * @note In debug mode, an informational message will be logged when a scheduler is deleted.
     */
    void DeleteScheduler(std::type_index id);

    /**
     * @brief Checks if a scheduler of the specified type exists in the container.
     *
     * This function checks if a scheduler of the specified type TScheduler
     * exists in the container. It returns true if the scheduler is found,
     * otherwise false.
     *
     * @param id The type index of the scheduler to check for.
     * @return true if the scheduler exists, false otherwise.
     */
    template <typename TScheduler> bool Contains() const;

    /**
     * @brief Checks if a scheduler identified by the given type index exists in the container.
     *
     * This function checks if a scheduler associated with the specified type index exists in the container.
     * It returns true if the scheduler is found, otherwise false.
     *
     * @param id The type index of the scheduler to check for.
     * @return true if the scheduler exists, false otherwise.
     */
    bool Contains(std::type_index id) const;

  private:
    std::unordered_map<std::type_index, std::size_t>
        _idToIndex; ///< Map to store unique ids for each scheduler, mapping type_index to their position in the vector.
    // TODO: Have a better data structure to avoid moving all the elements when deleting a scheduler.
    SchedulerStorage _orderedSchedulers; ///< Vector to store schedulers in order.
};
} // namespace ES::Engine

#include "SchedulerContainer.inl"

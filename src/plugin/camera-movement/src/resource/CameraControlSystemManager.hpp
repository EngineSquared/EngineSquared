#pragma once

#include <memory>

#include "core/Core.hpp"
#include "entity/Entity.hpp"
#include "system/CameraControlSystem.hpp"

namespace CameraMovement::Resource {

/**
 * @brief CameraControlSystemManager is a resource that helps to manage the CameraControlSystem.
 * 
 * Namely, it stores the FunctionID of the system to allow changing the scheduler it is registered to.
 */
class CameraControlSystemManager {
    public:
        explicit CameraControlSystemManager(Engine::Core &core)
        {
            _cameraControlSystemID = std::get<0>(core.RegisterSystem<Engine::Scheduler::Update>(System::CameraControlSystem));
        }
        ~CameraControlSystemManager()
        {

        }

        /**
         * @brief Set the CameraControlSystem to use a different scheduler.
         * 
         * @tparam TScheduler The type of the scheduler to move the system to.
         * @param core The core instance.
         */
        template <Engine::CScheduler TScheduler>
        void SetCameraControlSystemScheduler(Engine::Core &core)
        {
            core.GetScheduler(_currentScheduler).Disable(_cameraControlSystemID);
            _cameraControlSystemID = std::get<0>(core.RegisterSystem<TScheduler>(System::CameraControlSystem));
            _currentScheduler = std::type_index(typeid(TScheduler));
        }
    
    private:
        FunctionUtils::FunctionID _cameraControlSystemID;
        std::type_index _currentScheduler = typeid(Engine::Scheduler::Update);
};

}
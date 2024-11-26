#pragma once

namespace ES::Engine {
/**
 * Label used to schedule systems. It can be fixed or non fixed depending on the needs.
 *
 * @see ES::Engine::Registry::RegisterSystem
 */
enum class ScheduleLabel {
    NON_FIXED,
    FIXED
};
} // namespace ES::Engine

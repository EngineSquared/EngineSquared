#pragma once

#include <chrono>
#include <thread>

/**
 * @brief Sleep for a given amount of time in milliseconds
 * @param milliseconds Time to sleep in milliseconds
 * @note This should be crossplatform given it uses cpp standard library functions
 */
inline void SleepFor(unsigned long milliseconds)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

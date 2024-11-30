#include "Clock.hpp"

void ES::Utils::Clock::Update()
{
    auto currentTime = std::chrono::high_resolution_clock::now();
    this->_elapsedTime += std::chrono::duration<float>(currentTime - this->_lastTime).count();
    this->_elapsedTicks = static_cast<unsigned int>(this->_elapsedTime / this->_tickRate);
    this->_elapsedTime -= static_cast<float>(this->_elapsedTicks) * this->_tickRate;
    this->_lastTime = currentTime;
}

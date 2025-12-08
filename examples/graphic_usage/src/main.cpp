/**************************************************************************
 * EngineSquared - Graphic Usage Example
 *
 * This example demonstrates how to use the Graphic plugin.
 **************************************************************************/

#include "Engine.hpp"

#include "Graphic.hpp"

int main(void)
{
    Engine::Core core;

    core.AddPlugins<Window::Plugin, Graphic::Plugin>();

    core.RunCore();

    return 0;
}

#pragma once

#include "APlugin.hpp"

namespace ES::Plugin::RenderingPipeline {
/**
 * @brief This plugin create multiple schedulers that allows to create a rendering pipeline.
 * It will add the following "startup" pipeline:
 * - Init: Init libs like GLFW, GLEW, etc.
 * - Setup: Setup those libs.
 * - Startup: Here for engine's user to add their own startup code.
 * And the following "update" pipeline:
 * - PreUpdate: Used for player's event handling like updating mouse position, buttons'state, etc.
 * - Update: Here for engine's user to add their own update code.
 * - RenderSetup: Setup the rendering pipeline, like clearing the screen, setting up the camera, etc.
 * - ToGPU: Used to send the data to the GPU, like meshes, text, sprites, etc.
 * - Draw: Used for swapping the buffers and drawing the data on the screen. 
 */
  class Plugin : public ES::Engine::APlugin {
  public:
    using APlugin::APlugin;
    virtual ~Plugin() = default;

    void Bind() final;
};
} // namespace ES::Plugin::RenderingPipeline

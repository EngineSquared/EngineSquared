target("CharacterControllerUsage")
    set_kind("binary")
    set_default(true)
    add_deps("EngineSquaredCore")
    add_deps("PluginGraphic")
    add_deps("PluginWindow")
    add_deps("PluginInput")
    add_deps("PluginCameraMovement")
    add_deps("PluginPhysics")
    add_deps("PluginEvent")
    add_deps("PluginDefaultPipeline")
    add_deps("PluginObject")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native",
                 "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

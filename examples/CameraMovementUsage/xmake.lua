target("CameraMovementUsage")
    set_kind("binary")
    set_default(true)
    add_deps("PluginCameraMovement")
    add_deps("PluginDefaultPipeline")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native",
                 "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

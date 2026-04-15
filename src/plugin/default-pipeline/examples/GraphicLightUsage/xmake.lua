target("GraphicLightUsage")
    set_kind("binary")

    add_deps("PluginDefaultPipeline")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

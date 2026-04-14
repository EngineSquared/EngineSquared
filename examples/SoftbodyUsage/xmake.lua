target("SoftbodyUsage")
    set_kind("binary")

    add_deps("EngineSquared")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

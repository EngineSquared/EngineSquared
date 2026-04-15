target("PhysicsUsage")
    set_kind("binary")
    set_default(true)
    add_deps("PluginPhysics")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader")

    set_rundir("$(projectdir)")

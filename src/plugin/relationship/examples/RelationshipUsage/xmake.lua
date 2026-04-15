target("RelationshipUsage")
    set_kind("binary")
    set_default(true)
    add_deps("PluginRelationship")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "glew", "spdlog", "fmt", "joltphysics", "stb")

    set_rundir("$(projectdir)")

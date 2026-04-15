add_requires("sfml 3.0.1", {debug = is_mode("debug")})

target("Sfml")
    set_kind("binary")
    add_deps("EngineSquaredCore")
    add_deps("PluginEvent")
    add_deps("PluginObject")

    add_packages("sfml")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")
    add_packages("entt", "spdlog", "fmt", "glm", "tinyobjloader")
    set_rundir("$(projectdir)")

    if is_plat("macosx") then
        add_ldflags("-ObjC")
        add_frameworks("Carbon")
    end


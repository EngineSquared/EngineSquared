-- Build with xmake -P .

set_project("CharacterControllerUsage")
set_languages("c++20")

add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    add_cxflags("/W4")
end

target("CharacterControllerUsage")
    set_kind("binary")
    set_default(true)
    add_deps("EngineSquaredCore")
    add_deps("PluginPhysics")
    add_deps("PluginObject")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader")

    set_rundir("$(projectdir)")

if is_mode("debug") then
    add_defines("ES_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end

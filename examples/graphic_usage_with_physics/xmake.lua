-- Build with xmake -P .
set_project("GraphicUsageWithPhysics")
set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/W4")
end

-- includes("../../xmake.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("GraphicUsageWithPhysics")
    set_kind("binary")

    add_deps("EngineSquaredCore")
    add_deps("PluginGraphic")
    add_deps("PluginWindow")
    add_deps("PluginInput")
    add_deps("PluginCameraMovement")
    add_deps("PluginPhysics")
    add_deps("PluginEvent")
    add_deps("PluginDefaultPipeline")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

if is_mode("debug") then
    add_defines("ES_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end

-- Build with xmake -P .
set_project("GraphicLightUsage")
set_languages("c++20")
add_rules("mode.debug", "mode.release")

if is_plat("windows") then
    add_cxflags("/W4")
end

includes("../../xmake.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("GraphicLightUsage")
    set_kind("binary")

    add_deps("EngineSquared")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

if is_mode("debug") then
    add_defines("ES_DEBUG")
    set_symbols("debug")
    set_optimize("none")
end

if is_mode("release") then
    set_optimize("fastest")
end

-- Build with xmake -P .
set_project("RmluiUsage")
set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/W4")
end


includes("../../xmake.lua")

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})

local function setup_demo_target(target_name, source_file)
    target(target_name)
        set_kind("binary")
        set_languages("c++20")

        add_deps("EngineSquaredCore")
        add_deps("PluginGraphic")
        add_deps("PluginWindow")
        add_deps("PluginInput")
        add_deps("PluginCameraMovement")
        add_deps("EngineSquared")

        add_files(source_file)

        add_includedirs("$(projectdir)/src/")
        add_includedirs("$(projectdir)/asset/")

        add_packages("entt", "glm", "glfw", "spdlog", "fmt", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng", "rmlui")
        add_linkdirs("$(package:rmlui):installdir()/lib")
        add_links("rmlui_debugger", "rmlui")
        if is_plat("linux") then
            add_ldflags("-Wl,--start-group", "-lrmlui_debugger", "-lrmlui", "-Wl,--end-group", {force = true})
        end

        set_rundir("$(projectdir)")

    if is_mode("debug") then
        add_defines("ES_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end

    if is_mode("release") then
        set_optimize("fastest")
    end
end

setup_demo_target("RmluiUsage", "src/main.cpp")
setup_demo_target("RmluiUsageAnimation", "src/Animation.cpp")
setup_demo_target("RmluiUsageDemo", "src/Demo.cpp")
setup_demo_target("RmluiUsageTransform", "src/Transform.cpp")

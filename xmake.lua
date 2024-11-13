add_rules("mode.debug", "mode.release")
add_requires("entt", "vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "gtest")

includes("src/plugin/camera/xmake.lua")
includes("src/plugin/colors/xmake.lua")
includes("src/plugin/input/xmake.lua")
includes("src/plugin/math/xmake.lua")
includes("src/plugin/object/xmake.lua")
includes("src/plugin/physics/xmake.lua")
includes("src/plugin/scene/xmake.lua")
includes("src/plugin/time/xmake.lua")
includes("src/plugin/ui/xmake.lua")
includes("src/plugin/utils/xmake.lua")
includes("src/plugin/vk-wrapper/xmake.lua")
includes("src/plugin/window/xmake.lua")
includes("src/engine/xmake.lua")

add_rules("plugin.vsxmake.autoupdate")
target("EngineSquared")
    set_kind("static")
    set_default(true)
    set_languages("cxx20")

    add_deps("EngineSquaredCore")

    add_deps("PluginCamera")
    add_deps("PluginColors")
    add_deps("PluginInput")
    add_deps("PluginMath")
    add_deps("PluginObject")
    add_deps("PluginPhysics")
    add_deps("PluginScene")
    add_deps("PluginTime")
    add_deps("PluginUI")
    add_deps("PluginUtils")
    add_deps("PluginVkWrapper")
    add_deps("PluginWindow")

    set_policy("build.warning", true)
    add_packages("entt", "vulkansdk", "glfw", "glm")

    if is_mode("debug") then
        add_defines("DEBUG")
        if is_plat("windows") then
            add_cxflags("/Od", "/Zi")
        else
            add_cxflags("-O0 -g3 -ggdb")
        end
    else
        add_defines("NDEBUG")
        add_cxflags("-O2")
    end

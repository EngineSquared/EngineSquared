add_rules("mode.debug", "mode.release")
add_requires("entt", "glm", "gtest")

includes("src/plugin/camera/xmake.lua")
includes("src/plugin/collision/xmake.lua")
includes("src/plugin/input/xmake.lua")
includes("src/plugin/object/xmake.lua")
includes("src/plugin/physics/xmake.lua")
includes("src/plugin/time/xmake.lua")
includes("src/plugin/window/xmake.lua")
includes("src/engine/xmake.lua")

add_rules("plugin.vsxmake.autoupdate")
target("EngineSquared")
    set_kind("static")
    set_default(true)
    set_languages("cxx20")

    add_deps("EngineSquaredCore")
    add_deps("PluginCamera")
    add_deps("PluginCollision")
    add_deps("PluginInput")
    add_deps("PluginObject")
    add_deps("PluginPhysics")
    add_deps("PluginTime")
    add_deps("PluginWindow")

    set_policy("build.warning", true)
    add_packages("entt", "glm")

    if is_mode("debug") then
        add_defines("DEBUG")
    end
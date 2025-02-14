add_rules("mode.debug", "mode.release")
add_requires("entt", "vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "gtest", "spdlog", "tinyobjloader")

includes("src/plugin/camera/xmake.lua")
includes("src/plugin/colors/xmake.lua")
includes("src/plugin/input/xmake.lua")
includes("src/plugin/math/xmake.lua")
includes("src/plugin/native-scripting/xmake.lua")
includes("src/plugin/object/xmake.lua")
includes("src/plugin/physics/xmake.lua")
includes("src/plugin/scene/xmake.lua")
includes("src/plugin/ui/xmake.lua")
includes("src/plugin/utils/xmake.lua")
includes("src/plugin/vk-wrapper/xmake.lua")
includes("src/plugin/window/xmake.lua")
includes("src/utils/log/xmake.lua")
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
    add_deps("PluginUI")
    add_deps("PluginUtils")
    add_deps("PluginVkWrapper")
    add_deps("PluginWindow")
    add_deps("UtilsLog")

    set_policy("build.warning", true)
    add_packages("entt", "vulkansdk", "glfw", "glm", "spdlog", "tinyobjloader")

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

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_kind("binary")
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end
        set_default(false)
        set_languages("cxx20")
        add_files(file)
        add_files("tests/main.cpp")
        add_packages("entt", "vulkansdk", "glfw", "glm", "gtest", "spdlog", "tinyobjloader")
        add_links("gtest")
        add_deps("EngineSquared")
        add_includedirs("src")
        add_includedirs("tests")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

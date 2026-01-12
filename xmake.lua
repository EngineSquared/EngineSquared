TEST_GROUP_NAME = "UnitTests"
PLUGINS_GROUP_NAME = "Plugins"
UTILS_GROUP_NAME = "Utils"
-- Set the default group for all targets

add_rules("mode.debug", "mode.release")
add_requires(
    "entt v3.15.0",
    "gtest v1.17.0",
    "spdlog v1.16.0",
    "tinyobjloader v2.0.0rc13",
    "glm 1.0.1",
    "glfw 3.4",
    "fmt 12.1.0",
    "stb 2025.03.14",
    "joltphysics v5.4.0",
    "miniaudio 0.11.23",
    "lodepng 2025.05.06",
    "wgpu-native ^24.0.0",
    "glfw3webgpu v1.3.0-alpha",
    { debug = is_mode("debug") }
)
-- Temporarily disabled due to build issues on Windows 11
--add_requires("rmlui 6.0", { configs = { transform = true }, debug = is_mode("debug") })

set_languages("c++20")

option("with_graphic_usage_example", {default = false, description = "Enable Graphic Usage Example"})

if has_config("with_graphic_usage_example") then
    includes("examples/graphic_usage/xmake.lua")
end

includes("src/engine/xmake.lua")
includes("src/plugin/input/xmake.lua")
includes("src/plugin/native-scripting/xmake.lua")
includes("src/plugin/object/xmake.lua")
includes("src/plugin/relationship/xmake.lua")
includes("src/plugin/scene/xmake.lua")
includes("src/plugin/sound/xmake.lua")
includes("src/plugin/window/xmake.lua")
includes("src/plugin/rendering-pipeline/xmake.lua")
includes("src/plugin/graphic/xmake.lua")
includes("src/plugin/physics/xmake.lua")
includes("src/utils/function-container/xmake.lua")
includes("src/utils/log/xmake.lua")
includes("src/utils/tools/xmake.lua")
includes("src/plugin/event/xmake.lua")
includes("src/plugin/default-pipeline/xmake.lua")

add_rules("plugin.vsxmake.autoupdate")
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("EngineSquared")
    set_kind("object")
    set_version("0.0.0")

    add_deps("EngineSquaredCore")

    add_deps("PluginInput")
    add_deps("PluginObject")
    add_deps("PluginScene")
    add_deps("PluginSound")
    add_deps("PluginWindow")
    add_deps("PluginRelationship")
    add_deps("PluginNativeScripting")
    add_deps("PluginRenderingPipeline")
    add_deps("PluginGraphic")
    add_deps("UtilsTools")
    add_deps("PluginPhysics")
    add_deps("UtilsLog")
    add_deps("PluginEvent")
    add_deps("PluginDefaultPipeline")

    add_packages("entt", "glfw", "glm", "spdlog", "tinyobjloader", "fmt", "stb", "joltphysics", "wgpu-native")

    if is_mode("debug") then
        add_defines("DEBUG")
        add_defines("ES_DEBUG")
        if is_plat("windows") then
            add_cxflags("/Od", "/Zi", "/Wall", "/MTd")
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
        set_languages("cxx20")
        add_files(file)
        add_files("tests/main.cpp")
        add_packages("entt", "glfw", "glm", "gtest", "spdlog", "tinyobjloader", "fmt")
        add_links("gtest")
        add_deps("EngineSquared")
        add_includedirs("src")
        add_includedirs("tests")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
            add_defines("ES_DEBUG")
        end
    ::continue::
end

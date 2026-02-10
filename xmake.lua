---@diagnostic disable: undefined-global

TEST_GROUP_NAME = "UnitTests"
PLUGINS_GROUP_NAME = "Plugins"
UTILS_GROUP_NAME = "Utils"

add_rules("mode.debug", "mode.release")
add_requires(
    "entt v3.15.0",
    "gtest v1.17.0",
    "spdlog v1.16.0",
    "tinyobjloader v2.0.0rc13",
    "glm 1.0.1",
    "glfw 3.4",
    "freetype 2.14.1",
    "zlib 1.3.1",
    "stb 2025.03.14",
    "miniaudio 0.11.23",
    "lodepng 2025.05.06",
    "wgpu-native ^24.0.0",
    "glfw3webgpu v1.3.0-alpha",
    { debug = is_mode("debug") }
)

add_requires("joltphysics v5.4.0", { configs = { symbols = is_mode("debug") }, debug = is_mode("debug") })
add_requires("fmt 12.1.0", { configs = { header_only = true }, debug = is_mode("debug") })
add_requires("rmlui 6.2", { configs = { transform = true, font_effects = true }, debug = is_mode("debug") })

set_languages("c++20")

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
includes("src/plugin/camera-movement/xmake.lua")
includes("src/plugin/default-pipeline/xmake.lua")
includes("src/plugin/rmlui/xmake.lua")

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
    add_deps("PluginCameraMovement")
    add_deps("UtilsLog")
    add_deps("PluginEvent")
    add_deps("PluginDefaultPipeline")
    add_deps("PluginRmlui")

    add_packages("entt", "glfw", "glm", "spdlog", "tinyobjloader", "fmt", "stb", "joltphysics", "wgpu-native",
                 "rmlui", "freetype", "zlib")
    if is_plat("windows") then
        add_links("freetype", "zlib")
    else
        add_links("freetype", "z")
    end

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
target_end()

local ALL_EXAMPLES_FLAG_NAME = "all_examples"
local EXECUTABLE_EXAMPLES_FLAG_NAME = "executable_examples"

option(ALL_EXAMPLES_FLAG_NAME, {default = false, description = "Enable all examples"})
option(EXECUTABLE_EXAMPLES_FLAG_NAME, {default = false, description = "Enable executable examples"})

for _, dir in ipairs(os.dirs("examples/*")) do
    local name = path.basename(dir)

    option(name, {default = false, description = "Enable \"" .. name .. "\" Example"})

    if has_config(ALL_EXAMPLES_FLAG_NAME) or has_config(name) or has_config(EXECUTABLE_EXAMPLES_FLAG_NAME) then
        if (not os.isfile(path.join(dir, "xmake.lua"))) then
            print("Warning: No xmake.lua found in " .. dir .. ", skipping...")
        else
            if has_config(EXECUTABLE_EXAMPLES_FLAG_NAME) and os.isfile(path.join("examples", name, ".ci_run_target")) then
                includes(path.join("examples", name, "xmake.lua"))
            elseif has_config(ALL_EXAMPLES_FLAG_NAME) or not has_config(EXECUTABLE_EXAMPLES_FLAG_NAME) then
                includes(path.join("examples", name, "xmake.lua"))
            end
        end
    end
end

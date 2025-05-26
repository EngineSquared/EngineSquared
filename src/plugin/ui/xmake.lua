add_rules("mode.debug", "mode.release")
add_requires("entt", "glfw >=3.4", "rmlui", "glew", "fmt", "spdlog", "glm", "stb")
add_requires("gtest", {optional = true})

includes("../../engine/xmake.lua")
includes("../../utils/tools/xmake.lua")
includes("../colors/xmake.lua")
includes("../math/xmake.lua")
includes("../input/xmake.lua")
includes("../object/xmake.lua")
includes("../physics/xmake.lua")
includes("../opengl/xmake.lua")
includes("../window/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("PluginUI")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt", "glfw", "glm", "rmlui", "fmt", "spdlog", "glew", "stb")

    set_pcxxheader("src/UI.pch.hpp")

    add_deps("EngineSquaredCore")
    add_deps("PluginColors")
    add_deps("UtilsTools")
    add_deps("PluginMath")
    add_deps("PluginInput")
    add_deps("PluginPhysics")
    add_deps("PluginOpenGL")
    add_deps("PluginWindow")

    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
    add_includedirs("src/component", {public = true})
    add_includedirs("src/plugin", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/system", {public = true})
    add_includedirs("src/utils", {public = true})

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_group(TEST_GROUP_NAME)
        set_kind("binary")
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end
        set_default(false)
        set_languages("cxx20")
        add_links("gtest")
        add_tests("default")
        add_packages("glm", "entt", "gtest", "spdlog", "fmt")
        
        add_deps("PluginUI")
        add_deps("EngineSquaredCore")
        add_deps("UtilsTools")
        add_deps("PluginObject")
        
        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

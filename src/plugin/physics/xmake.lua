add_rules("mode.debug", "mode.release")
add_requires("glm", "entt", "gtest", "fmt", "spdlog", "joltphysics")

includes("../../engine/xmake.lua")
includes("../object/xmake.lua")
includes("../math/xmake.lua")
includes("../input/xmake.lua")
includes("../opengl/xmake.lua")

target("PluginPhysics")
    set_group(PLUGINS_GROUP_NAME)
    set_kind("static")
    set_languages("cxx20")
    add_packages("glm", "entt", "fmt", "spdlog", "joltphysics")
    set_policy("build.warning", true)

    set_pcxxheader("src/JoltPhysics.pch.hpp")

    add_deps("EngineSquaredCore")
    add_deps("PluginObject")
    add_deps("PluginMath")
    add_deps("PluginInput")
    add_deps("PluginOpenGL")

    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
    add_includedirs("src/component", {public = true})
    add_includedirs("src/plugin", {public = true})
    add_includedirs("src/system", {public = true})
    add_includedirs("src/utils", {public = true})
    add_includedirs("src/resource", {public = true})

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
        add_deps("EngineSquaredCore")
        add_files(file)
        add_files("tests/main.cpp")
        add_includedirs("tests/utils", {public = true})
        add_packages("glm", "entt", "gtest", "fmt", "spdlog", "joltphysics")
        add_links("gtest")
        add_deps("PluginPhysics")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

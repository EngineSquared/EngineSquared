includes("../../engine/xmake.lua")
includes("../input/xmake.lua")
includes("../object/xmake.lua")
includes("../window/xmake.lua")
includes("../colors/xmake.lua")
includes("../math/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("StbImage")
    set_kind("static")
    set_languages("cxx20")
    add_packages("stb")
    add_files("lib/stb_image/src/**.cpp")

    -- TODO: do we need this in this target or a global one works ?
    if is_mode("debug") then
        add_defines("DEBUG")
    end

target("PluginOpenGL")
    set_kind("static")
    set_languages("cxx20")
    set_group(PLUGINS_GROUP_NAME)
    add_packages("entt", "glm", "spdlog", "fmt", "glfw", "glew", "stb")

    set_pcxxheader("src/OpenGL.pch.hpp")

    -- TODO: do we need this in this target or a global one works ?
    if is_mode("debug") then
        add_defines("DEBUG")
    end

    add_deps("EngineSquaredCore")
    add_deps("PluginInput")
    add_deps("PluginObject")
    add_deps("PluginWindow")
    add_deps("PluginMath")
    add_deps("PluginColors")
    add_deps("PluginRenderingPipeline")
    add_deps("StbImage")

    add_files("src/**.cpp")

    add_headerfiles("src/(component/*.hpp)")
    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src/", {public = true})

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

        set_languages("cxx20")
        add_links("gtest")
        add_tests("default")
        add_packages("entt", "glm", "spdlog", "fmt", "glfw", "glew", "gtest")

        add_deps("OpenGL")

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

add_requires("entt", "gtest", "glm", "spdlog", "fmt", "glfw >=3.4", "glew", "stb")

includes("../../engine/xmake.lua")
includes("../input/xmake.lua")
includes("../object/xmake.lua")
includes("../window/xmake.lua")
includes("../colors/xmake.lua")
includes("../math/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("PluginOpenGL")
    set_kind("static")
    set_languages("cxx20")
    add_packages("entt", "glm", "spdlog", "fmt", "glfw", "glew", "stb")
    set_policy("build.warning", true)

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

    add_files("src/**.cpp")

    add_includedirs("src/", {public = true})
    add_includedirs("src/exception/", {public = true})
    add_includedirs("src/utils/", {public = true})
    add_includedirs("src/component/", {public = true})
    add_includedirs("src/resource/", {public = true})
    add_includedirs("src/system/", {public = true})
    add_includedirs("src/plugin/", {public = true})
    add_headerfiles("src/**.hpp")

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
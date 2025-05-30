add_rules("mode.debug", "mode.release")
add_requires("glfw", "glm", "entt", "spdlog", "fmt")

includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("PluginWindow")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")
    add_deps("PluginRenderingPipeline")

    add_packages("glfw", "glm", "entt", "spdlog", "fmt")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src/", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/resource/Window", {public = true})
    add_includedirs("src/exception", {public = true})
    add_includedirs("src/system", {public = true})
    add_includedirs("src/plugin", {public = true})

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

        add_deps("EngineSquaredCore")
        add_deps("PluginRenderingPipeline")
        add_deps("UtilsMock")
        add_deps("UtilsLog")
        add_deps("PluginWindow")

        add_packages("glfw", "entt", "glm", "spdlog", "fmt", "gtest")

        if is_plat("windows") then
            add_ldflags("/FORCE:MULTIPLE", {force = true})
        else
            add_ldflags("-Wl,--allow-multiple-definition", {force = true})
        end

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

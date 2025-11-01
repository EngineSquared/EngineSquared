includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../rendering-pipeline/xmake.lua")
includes("../window/xmake.lua")

local required_packages = {
    "entt",
    "spdlog",
    "fmt",
    "wgpu-native",
    "glfw",
    "glm",
    "glfw3webgpu"
}

local plugin_name = "PluginGraphic"

target(plugin_name)
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages(required_packages)

    add_deps("EngineSquaredCore")
    add_deps("PluginRenderingPipeline")
    add_deps("PluginWindow")
    add_deps("UtilsLog")

    add_files("src/**.cpp")

    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/initialization/*.hpp)")
    add_headerfiles("src/(system/shutdown/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    if path.filename(path.directory(file)) == "utils" then
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
        add_packages(required_packages, "gtest")

        add_deps(plugin_name)

        add_files("tests/utils/**.cpp")

        add_headerfiles("tests/(utils/*.hpp)")

        add_includedirs("tests", {public = true})

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

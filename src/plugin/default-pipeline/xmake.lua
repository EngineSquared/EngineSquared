includes("../../engine/xmake.lua")
includes("../graphic/xmake.lua")

local required_packages = {
    "entt",
    "spdlog",
    "fmt",
    "glm",
    "wgpu-native",
    "glfw",
    "glfw3webgpu",
    "stb",
    "lodepng",
    "tinyobjloader"
}

local plugin_name = "PluginDefaultPipeline"

local target_dependencies = {
    "EngineSquaredCore",
    "PluginGraphic"
}

target(plugin_name)
    set_group(PLUGINS_GROUP_NAME)
    set_kind("static")
    set_languages("cxx20")

    add_packages(required_packages)

    add_deps(target_dependencies)

    add_headerfiles("src/(component/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/*.hpp)")
    add_headerfiles("src/(system/preparation/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src/", {public = true})

    add_files("src/**.cpp")

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
        add_packages("gtest", required_packages)
        add_links("gtest")

        add_deps(plugin_name, target_dependencies)
        add_files(file)
        add_files("tests/main.cpp")

        add_tests("default")

        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

includes("../../engine/xmake.lua")

add_requires("nlohmann_json v3.12.0")
add_requires("dylib v3.0.1", {debug = is_mode("debug")})

local plugin_name = "PluginDynamicLibrary"

local required_packages = {
    "entt",
    "spdlog",
    "fmt",
    "nlohmann_json",
    "dylib"
}

-- includes("tests/zig_project/xmake.lua")
-- includes("tests/rust_project/xmake.lua")
includes("tests/cpp_project/xmake.lua")
includes("tests/c_project/xmake.lua")

target(plugin_name)
    set_group(PLUGINS_GROUP_NAME)
    set_kind("static")
    set_languages("cxx20")

    add_packages(required_packages)

    add_headerfiles("src/(*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/*.hpp)")

    -- add_deps("ZigLib", {inherit = false})
    -- add_deps("RustLib", {inherit = false})
    add_deps("CppLib", {inherit = false})
    add_deps("CLib", {inherit = false})

    add_deps("EngineSquaredCore")

    add_includedirs("src/", {public = true})

    add_files("src/**.cpp")

    set_rundir("$(projectdir)")

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_group(TEST_GROUP_NAME)
        set_kind("binary")
        set_default(false)
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end

        set_languages("cxx20")
        add_packages("gtest")
        add_packages(required_packages)
        add_links("gtest")
        add_tests("default")

        add_deps(plugin_name)

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end

        set_rundir("$(projectdir)")
    ::continue::
end

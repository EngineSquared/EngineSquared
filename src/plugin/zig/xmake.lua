includes("../../engine/xmake.lua")

local plugin_name = "PluginZig"

local required_packages = {
    "entt",
    "spdlog",
    "fmt"
}

target(plugin_name)
    set_group(PLUGINS_GROUP_NAME)
    set_kind("headeronly")
    set_languages("cxx20")

    add_packages(required_packages)

    add_headerfiles("src/(component/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

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

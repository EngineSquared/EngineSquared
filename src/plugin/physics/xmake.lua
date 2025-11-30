includes("../../engine/xmake.lua")
includes("../object/xmake.lua")
includes("../event/xmake.lua")

target("PluginPhysics")
    set_group(PLUGINS_GROUP_NAME)
    set_kind("static")
    set_languages("cxx20")
    add_packages("glm", "entt", "fmt", "spdlog", "joltphysics", "tinyobjloader")

    set_pcxxheader("src/Physics.pch.hpp")

    add_deps("EngineSquaredCore")
    add_deps("PluginObject")
    add_deps("PluginEvent")

    add_files("src/**.cpp")

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
        add_deps("EngineSquaredCore")
        add_files(file)
        add_files("tests/main.cpp")
        add_includedirs("tests/utils", {public = true})
        add_packages("glm", "entt", "gtest", "fmt", "spdlog", "joltphysics", "tinyobjloader")
        add_links("gtest")
        add_deps("PluginPhysics")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

includes("../../engine/xmake.lua")

target("PluginRenderingPipeline")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages("entt", "fmt", "spdlog")

    add_deps("EngineSquaredCore")

    add_files("src/**.cpp")

    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(scheduler/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

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
        add_links("gtest")
        add_tests("default")
        add_packages("entt", "spdlog", "fmt", "gtest")

        add_deps("PluginRenderingPipeline")

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

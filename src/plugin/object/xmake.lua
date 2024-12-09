add_rules("mode.debug", "mode.release")
add_requires("entt", "gtest", "glm", "tinyobjloader", "spdlog")

includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")

target("PluginObject")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt", "glm", "tinyobjloader", "spdlog")

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")

    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src/", {public = true})
    add_includedirs("src/component", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/utils", {public = true})

    add_files("src/**.cpp")

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
        add_packages("entt", "gtest", "glm", "tinyobjloader", "spdlog")
        add_links("gtest")
        add_tests("default")

        add_deps("PluginObject")
        add_deps("EngineSquaredCore")

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

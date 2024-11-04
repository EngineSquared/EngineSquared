add_rules("mode.debug", "mode.release")
add_requires("glm", "entt", "gtest")

includes("../../engine/xmake.lua")
includes("../time/xmake.lua")
includes("../object/xmake.lua")
includes("../collision/xmake.lua")

target("plugin_physics")
    set_kind("static")
    set_languages("cxx20")
    add_packages("glm", "entt")
    set_policy("build.warning", true)

    add_deps("engine_squared_core")
    add_deps("plugin_time")
    add_deps("plugin_object")
    add_deps("plugin_collision")
    
    add_files("src/**.cpp")
    add_includedirs("src/component", {public = true})
    add_includedirs("src/system", {public = true})

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
        add_deps("engine_squared_core")
        add_files(file)
        add_files("tests/main.cpp")
        add_includedirs("tests/utils", {public = true})
        add_packages("glm", "entt", "gtest")
        add_links("gtest")
        add_deps("plugin_physics")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

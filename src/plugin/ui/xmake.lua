add_rules("mode.debug", "mode.release")
add_requires("entt", "glm", "glfw")
add_requires("gtest", {optional = true})

includes("../../engine/xmake.lua")
includes("../collision/xmake.lua")
includes("../colors/xmake.lua")
includes("../utils/xmake.lua")
includes("../math/xmake.lua")
includes("../input/xmake.lua")
includes("../object/xmake.lua")

target("PluginUI")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt", "glm", "glfw")

    add_deps("EngineSquaredCore")
    add_deps("PluginCollision")
    add_deps("PluginColors")
    add_deps("PluginUtils")
    add_deps("PluginMath")
    add_deps("PluginInput")
    
    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
    add_includedirs("src/system", {public = true})
    add_includedirs("src/component", {public = true})

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
        add_packages("glm", "entt", "gtest")
        
        add_deps("PluginUI")
        add_deps("EngineSquaredCore")
        add_deps("PluginUtils")
        add_deps("PluginObject")
        
        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

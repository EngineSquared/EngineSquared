add_rules("mode.debug", "mode.release")
add_requires("entt", "vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "gtest")

includes("src/plugin/camera/xmake.lua")
includes("src/plugin/collision/xmake.lua")
includes("src/plugin/input/xmake.lua")
includes("src/plugin/object/xmake.lua")
includes("src/plugin/physics/xmake.lua")
includes("src/plugin/time/xmake.lua")
includes("src/plugin/window/xmake.lua")
includes("src/engine/xmake.lua")

add_rules("plugin.vsxmake.autoupdate")
target("engine_squared")
    set_kind("static")
    set_default(true)
    set_languages("cxx20")

    add_deps("engine_squared_core")

    add_deps("plugin_camera")
    add_deps("plugin_collision")
    add_deps("plugin_input")
    add_deps("plugin_object")
    add_deps("plugin_physics")
    add_deps("plugin_time")
    add_deps("plugin_window")

    set_policy("build.warning", true)
    add_packages("entt", "vulkansdk", "glfw", "glm")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

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
        add_files(file)
        add_files("tests/main.cpp")
        add_packages("entt", "vulkansdk", "glfw", "glm", "gtest")
        add_links("gtest")
        add_deps("engine_squared")
        add_includedirs("src")
        add_includedirs("tests")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

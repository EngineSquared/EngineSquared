-- Build with xmake -P .
set_project("RmluiUsage")
set_languages("c++20")

local required_packages = {
    "rmlui",
    "fmt",
    "glfw",
    "entt",
    "spdlog",
    "glm",
    "freetype",
    "zlib",
    "lodepng",
    "stb",
    "wgpu-native",
    "tinyobjloader",
    "glfw3webgpu",
}

local function setup_demo_target(target_name, source_file)
    target(target_name)
        set_kind("binary")
        set_languages("c++20")

        add_deps("EngineSquared")

        add_files(source_file)

        add_includedirs("src/")
        add_includedirs("asset/")

        add_packages(required_packages)
        add_links("rmlui_debugger", "rmlui")
        if is_plat("windows") then
            add_links("freetype", "zlib")
        else
            add_links("freetype", "z")
        end
        if is_plat("linux") then
            add_ldflags("-Wl,--start-group", "-lrmlui_debugger", "-lrmlui", "-lfreetype", "-lz", "-Wl,--end-group", {force = true})
        end

        set_rundir("$(projectdir)")

    if is_mode("debug") then
        add_defines("ES_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end

    if is_mode("release") then
        set_optimize("fastest")
    end
end

setup_demo_target("RmluiUsage", "src/main.cpp")
setup_demo_target("RmluiUsageAnimation", "src/Animation.cpp")
setup_demo_target("RmluiUsageDemo", "src/Demo.cpp")
setup_demo_target("RmluiUsageTransform", "src/Transform.cpp")

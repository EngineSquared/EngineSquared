includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../object/xmake.lua")
includes("../rendering-pipeline/xmake.lua")
includes("../window/xmake.lua")
includes("../event/xmake.lua")
includes("../input/xmake.lua")

local required_packages = {
    "entt",
    "spdlog",
    "fmt",
    "wgpu-native",
    "glfw",
    "glm",
    "glfw3webgpu",
    "stb",
    "lodepng"
}

local plugin_name = "PluginCameraMovement"

target(plugin_name)
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages(required_packages)

    add_deps("EngineSquaredCore")
    add_deps("PluginObject")
    add_deps("PluginRenderingPipeline")
    add_deps("PluginWindow")
    add_deps("PluginEvent")
    add_deps("UtilsLog")
    add_deps("PluginInput")
    add_files("src/**.cpp")

    add_headerfiles("src/(component/*.hpp)")
    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

target("PluginCameraMovementTests")
    set_kind("static")
    set_group(TEST_GROUP_NAME)
    set_languages("cxx20")

    add_packages(required_packages, "gtest")

    add_deps(plugin_name)

    add_files("tests/utils/**.cpp")

    add_headerfiles("tests/(utils/*.hpp)")

    add_includedirs("tests", {public = true})

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    if path.filename(path.directory(file)) == "utils" then
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
        add_links("gtest")
        add_tests("default")
        add_packages(required_packages, "gtest")

        add_deps(plugin_name)
        add_deps("PluginCameraMovementTests")

        add_includedirs("tests", {public = true})

        after_build(function (target)
            import("core.project.config")

            local builddir = config.get("builddir")
            if not builddir then
                builddir = config.get("buildir")
            end
            local targetdir = path.join(builddir, "$(plat)", "$(arch)", "$(mode)")
            local assets_files = os.files("$(scriptdir)" .. "/tests/assets/*")

            os.mkdir(path.join(targetdir, "assets"))

            for _, assets_file in ipairs(assets_files) do
                os.cp(assets_file, path.join(targetdir, "assets"))
            end
        end)

        on_clean(function (target)
            import("core.project.config")

            local builddir = config.get("builddir")
            if not builddir then
                builddir = config.get("buildir")
            end
            local targetdir = path.join(builddir, "$(plat)", "$(arch)", "$(mode)")
            os.rm(path.join(targetdir, "assets"))
        end)

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

-- Build with xmake -P .
set_project("SoftBodyUsage")
set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/W4")
end

add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("SoftBodyUsage")
    set_kind("binary")

    add_deps("EngineSquaredCore")
    add_deps("PluginGraphic")
    add_deps("PluginWindow")
    add_deps("PluginInput")
    add_deps("PluginPhysics")
    add_deps("PluginEvent")
    add_deps("UtilsTools")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "joltphysics", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir("$(projectdir)")

    after_build(function (target)
        import("core.project.config")

        local builddir = config.get("builddir")
        if not builddir then
            builddir = config.get("buildir")
        end
        local targetdir = path.join(builddir, "$(plat)", "$(arch)", "$(mode)")
        local assets_files = os.files("$(scriptdir)" .. "/assets/*")

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

    if is_mode("debug") then
        add_defines("ES_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    end

    if is_mode("release") then
        set_optimize("fastest")
    end
target_end()

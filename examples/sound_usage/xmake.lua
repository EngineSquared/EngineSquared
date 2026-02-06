-- Build with xmake -P .
set_project("SoundUsage")
set_languages("c++20")

if is_plat("windows") then
    add_cxflags("/W4")
end


add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("SoundUsage")
    set_kind("binary")

    add_deps("EngineSquared")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "spdlog", "fmt", "stb", "miniaudio")

    set_rundir("$(projectdir)")

    add_defines("PATH_ASSETS=\"$(builddir)/assets/\"")

    after_build(function (target)
        import("core.project.config")

        local builddir = config.get("builddir")
        if not builddir then
            builddir = config.get("buildir")
        end
        local assets_files = os.files("$(scriptdir)" .. "/assets/*")

        os.mkdir(path.join(builddir, "assets"))

        for _, assets_file in ipairs(assets_files) do
            os.cp(assets_file, path.join(builddir, "assets"))
        end
    end)

    on_clean(function (target)
        import("core.project.config")

        local builddir = config.get("builddir")
        if not builddir then
            builddir = config.get("buildir")
        end
        os.rm(path.join(builddir, "assets"))
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

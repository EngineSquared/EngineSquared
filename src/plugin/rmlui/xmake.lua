local plugin_name = "PluginRmlui"

local required_packages = {
    "rmlui",
    "fmt",
    "glfw",
    "entt",
    "spdlog",
    "glm",
    "lodepng",
    "stb",
    "wgpu-native"
}

local target_dependencies = {
    "EngineSquaredCore",
    "UtilsLog",
    "PluginWindow",
    "PluginInput",
    "PluginGraphic"
}
target("PluginRmlui")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    add_packages(required_packages)
    add_linkdirs("$(package:rmlui):installdir()/lib")
    add_links("rmlui_debugger", "rmlui")
    if is_plat("linux") then
        add_ldflags("-Wl,--start-group", "-lrmlui_debugger", "-lrmlui", "-Wl,--end-group", {force = true})
    end
    add_deps(target_dependencies)

    add_files("src/**.cpp")

    add_headerfiles("src/(**.hpp)")
    add_headerfiles("src/(plugin/**.hpp)")
    add_headerfiles("src/(system/**.hpp)")
    add_headerfiles("src/(resource/**.hpp)")
    add_headerfiles("src/(utils/**.hpp)")

    add_includedirs("src", {public = true})

target("PluginRmluiTests")
    set_kind("static")
    set_group(TEST_GROUP_NAME)
    set_languages("cxx20")

    add_packages(required_packages, "gtest")

    add_deps(target_dependencies)

    add_deps(plugin_name)

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
        add_deps("PluginGraphicTests")

        add_includedirs("tests", {public = true})

        after_build(function (target)
            import("core.project.config")

            local builddir = config.get("builddir")
            if not builddir then
                builddir = config.get("buildir")
            end
            local targetdir = path.join(builddir, "$(plat)", "$(arch)", "$(mode)")
            local assets_files = os.files("$(scriptdir)" .. "/tests/asset/*")

            os.mkdir(path.join(targetdir, "asset"))

            for _, assets_file in ipairs(assets_files) do
                os.cp(assets_file, path.join(targetdir, "asset"))
            end
        end)

        on_clean(function (target)
            import("core.project.config")

            local builddir = config.get("builddir")
            if not builddir then
                builddir = config.get("buildir")
            end
            local targetdir = path.join(builddir, "$(plat)", "$(arch)", "$(mode)")
            os.rm(path.join(targetdir, "asset"))
        end)

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end
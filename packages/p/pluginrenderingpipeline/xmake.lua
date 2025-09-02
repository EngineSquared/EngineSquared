package("pluginrenderingpipeline")
    set_description("The pluginrenderingpipeline package")

    add_deps("enginesquaredcore")
    add_deps("entt")
    add_deps("fmt")
    add_deps("spdlog")

    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        local libfiledir = (package:config("shared") and package:is_plat("windows", "mingw")) and "bin" or "lib"
        result.links = "PluginRenderingPipeline"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        result.libfiles = path.join(package:installdir(libfiledir), "libPluginRenderingPipeline.a")
        return result
    end)

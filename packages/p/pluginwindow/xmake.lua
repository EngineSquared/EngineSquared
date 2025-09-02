package("pluginwindow")
    set_description("The pluginwindow package")

    add_deps("enginesquaredcore", "utilslog", "pluginrenderingpipeline")
    add_deps("glfw >=3.4")
    add_deps("glm >=1.0.1")
    add_deps("entt")
    add_deps("spdlog")
    add_deps("fmt")

    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        local libfiledir = (package:config("shared") and package:is_plat("windows", "mingw")) and "bin" or "lib"
        result.links = "PluginWindow"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        result.libfiles = path.join(package:installdir(libfiledir), "libPluginWindow.a")
        return result
    end)

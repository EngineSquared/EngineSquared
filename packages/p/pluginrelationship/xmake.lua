package("pluginrelationship")
    set_description("The pluginrelationship package")

    add_deps("enginesquaredcore", "utilslog")
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
        result.links = "PluginRelationship"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        result.libfiles = path.join(package:installdir(libfiledir), "libPluginRelationship.a")
        return result
    end)

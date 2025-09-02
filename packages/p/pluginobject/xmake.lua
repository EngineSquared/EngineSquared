package("pluginobject")
    set_description("The pluginobject package")

    add_deps("enginesquaredcore", "utilslog")
    add_deps("entt")
    add_deps("glm >=1.0.1")
    add_deps("tinyobjloader")
    add_deps("spdlog")
    add_deps("fmt")

    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        local libfiledir = (package:config("shared") and package:is_plat("windows", "mingw")) and "bin" or "lib"
        result.links = "PluginObject"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        result.libfiles = path.join(package:installdir(libfiledir), "libPluginObject.a")
        return result
    end)

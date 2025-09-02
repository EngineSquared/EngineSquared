package("pluginui")
    set_description("The pluginui package")

    add_deps("enginesquaredcore", "plugincolors", "utilstools", "pluginmath", "plugininput", "pluginphysics", "pluginopengl", "pluginwindow")
    add_deps("entt")
    add_deps("glfw >=3.4")
    add_deps("glm >=1.0.1")
    add_deps("rmlui >=6.0", {configs={transform=true}})
    add_deps("fmt")
    add_deps("spdlog")
    add_deps("glew")
    add_deps("stb")

    add_configs("shared", {description = "Build shared library.", default = false, type = "boolean", readonly = true})

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        local libfiledir = (package:config("shared") and package:is_plat("windows", "mingw")) and "bin" or "lib"
        result.links = "PluginUI"
        result.linkdirs = package:installdir("lib")
        result.includedirs = package:installdir("include")
        result.libfiles = path.join(package:installdir(libfiledir), "libPluginUI.a")
        return result
    end)

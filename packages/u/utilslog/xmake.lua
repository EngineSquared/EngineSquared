package("utilslog")
    set_kind("library", {headeronly = true})
    set_description("The utilslog package")

    add_deps("spdlog")
    add_deps("fmt")

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        result.includedirs = package:installdir("include")
        return result
    end)

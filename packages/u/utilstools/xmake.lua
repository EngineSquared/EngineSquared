package("utilstools")
    set_kind("library", {headeronly = true})
    set_description("The utilstools package")

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)

    on_fetch(function (package)
        local result = {}
        result.includedirs = package:installdir("include")
        return result
    end)

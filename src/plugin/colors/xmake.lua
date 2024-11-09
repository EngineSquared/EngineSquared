add_rules("mode.debug", "mode.release")

target("PluginColors")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_includedirs("src/", {public = true})
    add_includedirs("src/utils", {public = true})
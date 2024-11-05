add_rules("mode.debug", "mode.release")

target("PluginInput")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    
    add_includedirs("./", {public = true})
    add_includedirs("resource", {public = true})
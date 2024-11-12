add_rules("mode.debug", "mode.release")


target("PluginUtils")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    
    add_headerfiles("src/**.h", { public = true })
    add_includedirs("src/", {public = true})
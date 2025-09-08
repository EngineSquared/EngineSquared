target("PluginColors")
    set_kind("headeronly")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")
    add_includedirs("src", {public = true})

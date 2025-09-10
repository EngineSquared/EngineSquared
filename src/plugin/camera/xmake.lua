target("PluginCamera")
    set_kind("headeronly")
    add_packages("glm")
    set_group(PLUGINS_GROUP_NAME)

    add_headerfiles("src/(component/*.hpp)")
    add_includedirs("src", {public = true})

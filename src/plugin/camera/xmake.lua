target("PluginCamera")
    set_kind("headeronly")
    add_packages("glm")
    set_group(PLUGINS_GROUP_NAME)

    add_headerfiles("src/**.hpp")
    add_includedirs("src", {public = true})
    add_includedirs("src/component", {public = true})

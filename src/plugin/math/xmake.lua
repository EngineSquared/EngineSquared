target("PluginMath")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages("glm")

    add_files("src/**.cpp")

    add_headerfiles("src/(*.hpp)")

    add_includedirs("src/", {public = true})

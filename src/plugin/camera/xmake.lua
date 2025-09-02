add_rules("mode.debug", "mode.release")
add_requires("glm")

target("PluginCamera")
    set_kind("headeronly")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    

    add_includedirs("src", {public = true})
    add_includedirs("src/component", {public = true})

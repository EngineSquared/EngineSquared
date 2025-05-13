add_rules("mode.debug", "mode.release")

target("PluginColors")
    set_kind("headeronly")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    set_policy("build.warning", true)

    set_pcxxheader("src/Colors.hpp")

    add_includedirs("src/", {public = true})
    add_includedirs("src/utils", {public = true})
add_rules("mode.debug", "mode.release")
add_requires("glm")

target("PluginCamera")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_includedirs("./", {public = true})
    add_includedirs("component", {public = true})
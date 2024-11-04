add_rules("mode.debug", "mode.release")
add_requires("glfw")

target("plugin_window")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_packages("glfw")
    
    add_files("**.cpp")
    add_includedirs("./", {public = true})
    add_includedirs("resource", {public = true})
    add_includedirs("exception", {public = true})
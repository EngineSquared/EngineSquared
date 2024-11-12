add_rules("mode.debug", "mode.release")
add_requires("spdlog")

target("PluginLog")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("spdlog")
    
    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
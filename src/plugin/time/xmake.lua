add_rules("mode.debug", "mode.release")
add_requires("entt")

includes("../../engine/xmake.lua")

target("PluginTime")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt")

    add_deps("EngineSquaredCore")
    
    add_files("**.cpp")
    add_includedirs("resource", {public = true})
    add_includedirs("system", {public = true})
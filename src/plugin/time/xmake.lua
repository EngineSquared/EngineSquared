add_rules("mode.debug", "mode.release")
add_requires("entt")

includes("../../engine/xmake.lua")

target("PluginTime")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt")

    add_deps("EngineSquaredCore")
    
    add_files("src/**.cpp")
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/system", {public = true})
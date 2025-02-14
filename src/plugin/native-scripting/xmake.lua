add_rules("mode.debug", "mode.release")
add_requires("entt")

includes("../../engine/xmake.lua")

target("PluginNativeScripting")
    set_kind("static")
    set_languages("cxx20")
    add_packages("entt")
    set_policy("build.warning", true)

    add_deps("EngineSquaredCore")

    add_files("src/**.cpp")
    add_includedirs("src", {public = true})
    add_includedirs("src/component", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/system", {public = true})
add_rules("mode.debug", "mode.release")
add_requires("entt", "fmt", "spdlog")

includes("../../engine/xmake.lua")

target("PluginRenderingPipeline")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt", "fmt", "spdlog")

    add_deps("EngineSquaredCore")
    
    add_files("src/**.cpp")
    add_includedirs("src", {public = true})
    add_includedirs("src/scheduler", {public = true})
    add_includedirs("src/plugin", {public = true})
add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "entt", "spdlog", "fmt")

includes("../../engine/xmake.lua")

target("PluginInput")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("glm", "glfw", "entt", "spdlog", "fmt")

    add_files("src/**.cpp")

    add_deps("EngineSquaredCore")

    add_includedirs("src", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/utils", {public = true})
    add_includedirs("src/plugin", {public = true})
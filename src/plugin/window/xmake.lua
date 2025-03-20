add_rules("mode.debug", "mode.release")
add_requires("glfw", "glm", "entt", "spdlog")

includes("../../engine/xmake.lua")

target("PluginWindow")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_deps("EngineSquaredCore")

    add_packages("glfw", "glm", "entt", "spdlog")

    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/exception", {public = true})
    add_includedirs("src/system", {public = true})

add_rules("mode.debug", "mode.release")
add_requires("glfw", "glm", "entt", "spdlog", "fmt")

includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("PluginWindow")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")
    add_deps("PluginRenderingPipeline")

    add_packages("glfw", "glm", "entt", "spdlog", "fmt")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src/", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/resource/Window", {public = true})
    add_includedirs("src/exception", {public = true})
    add_includedirs("src/system", {public = true})
    add_includedirs("src/plugin", {public = true})

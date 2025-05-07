add_rules("mode.debug", "mode.release")
add_requires("glm", "glfw", "entt", "spdlog", "fmt")

includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../../utils/function-container/xmake.lua")
includes("../window/xmake.lua")

target("PluginInput")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("glm", "glfw", "entt", "spdlog", "fmt")

    add_files("src/**.cpp")

    add_deps("EngineSquaredCore")
    add_deps("PluginWindow")
    add_deps("UtilsLog")
    add_deps("UtilsFunctionContainer")

    add_includedirs("src", {public = true})
    add_includedirs("src/exception", {public = true})
    add_includedirs("src/resource", {public = true})
    add_includedirs("src/utils", {public = true})
    add_includedirs("src/plugin", {public = true})
    add_includedirs("src/system", {public = true})
includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../rendering-pipeline/xmake.lua")

target("PluginWindow")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")
    add_deps("PluginRenderingPipeline")

    add_packages("glfw", "glm", "entt", "spdlog", "fmt")

    add_files("src/**.cpp")

    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(system/*.hpp)")

    add_includedirs("src", {public = true})

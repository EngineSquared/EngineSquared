includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")

target("PluginGraphic")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages("entt", "spdlog", "fmt")

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")

    add_files("src/**.cpp")

    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

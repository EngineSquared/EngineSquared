includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")
includes("../../utils/function-container/xmake.lua")
includes("../window/xmake.lua")

target("PluginInput")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages("glm", "glfw", "entt", "spdlog", "fmt")

    add_deps("EngineSquaredCore")
    add_deps("PluginWindow")
    add_deps("UtilsLog")
    add_deps("UtilsFunctionContainer")

    set_pcxxheader("src/Input.pch.hpp")

    add_files("src/**.cpp")

    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(utils/*.hpp)")
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

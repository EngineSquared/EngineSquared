target("PluginRmlui")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")
    add_packages("rmlui", "fmt", "glfw", "entt", "spdlog", "glm", "lodepng", "stb", "wgpu-native")
    add_linkdirs("$(package:rmlui):installdir()/lib")
    add_links("rmlui_debugger", "rmlui")
    if is_plat("linux") then
        add_ldflags("-Wl,--start-group", "-lrmlui_debugger", "-lrmlui", "-Wl,--end-group", {force = true})
    end
    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")
    add_deps("PluginWindow")
    add_deps("PluginInput")
    add_deps("PluginGraphic")

    add_files("src/**.cpp")

    add_headerfiles("src/(**.hpp)")
    add_headerfiles("src/(plugin/**.hpp)")
    add_headerfiles("src/(system/**.hpp)")
    add_headerfiles("src/(resource/**.hpp)")
    add_headerfiles("src/(utils/**.hpp)")

    add_includedirs("src", {public = true})
add_rules("plugin.compile_commands.autoupdate", {outputdir = ".vscode"})
target("SoundUsage")
    set_kind("binary")

    add_deps("PluginSound")

    add_files("src/**.cpp")

    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "spdlog", "fmt", "stb", "miniaudio")

    set_rundir("$(projectdir)")


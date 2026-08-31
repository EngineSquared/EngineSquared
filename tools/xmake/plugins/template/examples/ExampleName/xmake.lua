target("ExampleName")
    set_kind("binary")
    set_default(true)
    add_deps("PluginName")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")

    add_packages("entt", "spdlog", "fmt")

    set_rundir("$(projectdir)")

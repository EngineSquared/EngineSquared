target("BasicCoreUsage")
    set_kind("binary")
    add_deps("EngineSquaredCore")

    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")
    add_packages("entt", "glm", "spdlog", "fmt")
    set_rundir("$(projectdir)")

add_requires("ncurses 6.6", {debug = is_mode("debug")})

target("Ncurses")
    set_kind("binary")
    add_deps("EngineSquaredCore")

    add_packages("ncurses")
    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")
    add_packages("entt", "glm", "spdlog", "fmt")
    set_rundir("$(projectdir)")


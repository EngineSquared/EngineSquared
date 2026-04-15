add_requires("raylib 5.5", {debug = is_mode("debug")})

target("Raylib")
    set_kind("binary")
    add_deps("EngineSquaredCore")

    add_packages("raylib")
    add_files("src/**.cpp")
    add_includedirs("$(projectdir)/src/")
    add_packages("entt", "glm", "spdlog", "fmt")
    set_rundir("$(projectdir)")


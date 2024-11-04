add_rules("mode.debug", "mode.release")
add_requires("entt")

target("engine_squared_core")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt")
    add_files("**.cpp")
    add_headerfiles("/**.hpp", { public = true })
    add_includedirs(".", { public = true })
    add_includedirs("/entity", { public = true })
    add_includedirs("/registry", { public = true })

    if is_mode("debug") then
        add_defines("DEBUG")
    end
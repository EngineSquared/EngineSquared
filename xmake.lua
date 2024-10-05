add_rules("mode.debug", "mode.release")
add_requires("entt", "vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm")

add_rules("plugin.vsxmake.autoupdate")
target("EngineSquared")
    set_kind("static")
    set_default(true)
    set_languages("cxx20")
    add_files("src/*.cpp")
    add_files("src/**/*.cpp")
    add_includedirs("src")
    add_includedirs("src/engine", { public = true })
    add_includedirs("src/engine/entity", { public = true })
    add_includedirs("src/engine/registry", { public = true })
    
    set_policy("build.warning", true)
    add_packages("entt", "vulkansdk", "glfw", "glm")

    if is_mode("debug") then
        add_defines("DEBUG")
    end
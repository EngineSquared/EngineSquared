add_rules("mode.debug", "mode.release")
add_requires("entt", "vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "gtest")

add_rules("plugin.vsxmake.autoupdate")
target("EngineSquared")
    set_kind("static")
    set_default(true)
    set_languages("cxx20")
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src", { public = true })
    add_includedirs("src/engine", { public = true })
    add_includedirs("src/engine/entity", { public = true })
    add_includedirs("src/engine/registry", { public = true })
    add_includedirs("src/plugin", { public = true })
    add_includedirs("src/plugin/object", { public = true })
    add_includedirs("src/plugin/object/component", { public = true })
    add_includedirs("src/plugin/object/utils", { public = true })
    add_includedirs("src/plugin/object/resource", { public = true })
    add_includedirs("src/plugin/window/exception", { public = true })
    add_includedirs("src/plugin/window/resource", { public = true })
    add_includedirs("src/plugin/input/resource", { public = true })
    add_includedirs("src/plugin/camera/component", { public = true })
    add_includedirs("src/plugin/collision", { public = true })
    add_includedirs("src/plugin/collision/component", { public = true })
    add_includedirs("src/plugin/collision/system", { public = true })
    add_includedirs("src/plugin/collision/resource", { public = true })
    add_includedirs("src/plugin/time", { public = true })
    add_includedirs("src/plugin/time/resource", { public = true })
    add_includedirs("src/plugin/time/system", { public = true })
    add_includedirs("src/plugin/physics", { public = true })
    add_includedirs("src/plugin/physics/component", { public = true })
    add_includedirs("src/plugin/physics/resource", { public = true })
    add_includedirs("src/plugin/physics/system", { public = true })
    add_includedirs("src/plugin/time/resource", { public = true })
    add_includedirs("src/plugin/time/system", { public = true })

    set_policy("build.warning", true)
    add_packages("entt", "vulkansdk", "glfw", "glm")

    if is_mode("debug") then
        add_defines("DEBUG")
    end

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_kind("binary")
        set_default(false)
        set_languages("cxx20")
        add_files(file)
        add_files("tests/main.cpp")
        add_packages("entt", "vulkansdk", "glfw", "glm", "gtest")
        add_links("gtest")
        add_deps("EngineSquared")
        add_includedirs("src")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

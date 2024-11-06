add_rules("mode.debug", "mode.release")
add_requires("vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw")

includes("../utils/xmake.lua")

target("PluginVkWrapper")
    set_kind("static")
    set_languages("cxx20")
    add_packages("vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw")
    set_policy("build.warning", true)

    add_deps("PluginUtils")

    add_files("src/**.cpp")
    add_includedirs("src", { public = true })
    add_includedirs("src/instance", { public = true })
    add_includedirs("src/debugMessenger", { public = true })
    add_includedirs("src/device", { public = true })
    add_includedirs("src/queueFamilies", { public = true })
    add_includedirs("src/surface", { public = true })
    add_includedirs("src/swapChain", { public = true })
    add_includedirs("src/imageView", { public = true })
    add_includedirs("src/shaderModule", { public = true })
    add_includedirs("src/graphicsPipeline", { public = true })
    add_includedirs("src/renderPass", { public = true })
    add_includedirs("src/framebuffer", { public = true })
    add_includedirs("src/command", { public = true })
    add_includedirs("src/exception", { public = true })

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_kind("binary")
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end
        set_default(false)
        set_languages("cxx20")
        add_files(file)
        add_files("tests/main.cpp")
        add_includedirs("tests/utils", {public = true})
        add_packages("glm", "entt", "gtest")
        add_links("gtest")
        add_deps("PluginVkWrapper")
        add_tests("default")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

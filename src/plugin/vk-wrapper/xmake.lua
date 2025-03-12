add_rules("mode.debug", "mode.release")
add_requires("vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "spdlog", "stb", "entt")

includes("../utils/xmake.lua")
includes("../object/xmake.lua")
includes("../../utils/log/xmake.lua")

target("PluginVkWrapper")
    set_kind("static")
    set_languages("cxx20")
    add_packages("vulkan-headers", "vulkansdk", "vulkan-hpp", "glfw", "glm", "spdlog", "stb", "entt")
    set_policy("build.warning", true)
    set_version("0.0.0")

    if is_mode("debug") then
        add_defines("DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        add_defines("NDEBUG")
        set_optimize("fastest")
    end

    add_deps("PluginUtils")
    add_deps("PluginObject")
    add_deps("UtilsLog")

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
    add_includedirs("src/buffers", { public = true })
    add_includedirs("src/descriptor", { public = true })

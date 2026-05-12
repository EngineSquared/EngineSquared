local PLUGIN_DIR = os.scriptdir()

target("GraphicMaterialUsage")
    set_kind("binary")

    add_deps("PluginDefaultPipeline")

    add_files(PLUGIN_DIR .. "/src/**.cpp")

    add_includedirs(PLUGIN_DIR .. "/src/")

    add_packages("entt", "glm", "glfw", "spdlog", "fmt", "stb", "tinyobjloader", "wgpu-native", "glfw3webgpu", "lodepng")

    set_rundir(PLUGIN_DIR)

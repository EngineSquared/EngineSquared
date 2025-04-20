add_rules("mode.debug", "mode.release")
add_requires("entt", "spdlog", "fmt")

includes("../../engine/xmake.lua")

target("PluginNativeScripting")
    set_kind("moduleonly")
    set_languages("cxx20")
    add_packages("entt", "spdlog", "fmt")
    set_policy("build.warning", true)

    add_deps("EngineSquaredCore")

    add_files("src/**.mpp")

    -- if using gcc or clang, enable modules
    if (is_plat("linux") and is_toolchain("gcc", "clang")) then
        add_cxxflags("-fmodules-ts", {force = true})
    end


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
        add_links("gtest")
        add_tests("default")
        add_packages("glm", "entt", "gtest", "spdlog")

        add_deps("PluginNativeScripting")
        add_deps("EngineSquaredCore")

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end
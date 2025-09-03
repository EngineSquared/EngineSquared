add_rules("mode.debug", "mode.release")
add_requires("entt", "gtest", "glm", "tinyobjloader", "spdlog", "fmt")

includes("../../engine/xmake.lua")
includes("../../utils/log/xmake.lua")

target("PluginObject")
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages("entt", "glm", "tinyobjloader", "spdlog", "fmt")

    set_pcxxheader("src/Object.pch.hpp")

    add_deps("EngineSquaredCore")
    add_deps("UtilsLog")

    add_headerfiles("src/**.hpp")
    add_includedirs("src", {public = true})

    add_files("src/**.cpp")

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    target(name)
        set_group(TEST_GROUP_NAME)
        set_kind("binary")
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end
        
        set_languages("cxx20")
        add_packages("entt", "gtest", "glm", "tinyobjloader", "spdlog", "fmt")
        add_links("gtest")
        add_tests("default")

        add_deps("EngineSquaredCore")
        add_deps("PluginObject")
        add_deps("UtilsTools")

        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end

        after_build(function (target)
            local buildir = path.join("$(buildir)", "$(plat)", "$(arch)", "$(mode)")
            local assets_files = os.files("$(scriptdir)" .. "/tests/assets/*.*")

            os.mkdir(path.join(buildir, "assets"))

            for _, assets_file in ipairs(assets_files) do
                os.cp(assets_file, path.join(buildir, "assets"))
            end
        end)

        on_clean(function (target)
            os.rm(path.join("$(buildir)", "$(plat)", "$(arch)", "$(mode)", "assets"))
        end)
    ::continue::
end

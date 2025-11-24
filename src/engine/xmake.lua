add_rules("mode.debug", "mode.release")

add_requires("entt", "fmt", "spdlog", "gtest")

includes("../utils/log/xmake.lua")
includes("../utils/function-container/xmake.lua")

target("EngineSquaredCore")
    set_kind("static")
    set_languages("cxx20")

    add_packages("entt", "spdlog", "fmt")
    add_deps("UtilsLog")
    add_deps("UtilsFunctionContainer")

    set_pcxxheader("src/Engine.pch.hpp")

    add_files("src/**.cpp")

    add_headerfiles("src/(core/*.hpp)")
    add_headerfiles("src/(core/*.inl)")
    add_headerfiles("src/(entity/*.hpp)")
    add_headerfiles("src/(plugin/*.hpp)")
    add_headerfiles("src/(resource/*.hpp)")
    add_headerfiles("src/(scheduler/*.hpp)")
    add_headerfiles("src/(scheduler/*.inl)")
    add_headerfiles("src/(system/*.hpp)")
    add_headerfiles("src/(exception/*.hpp)")
    add_headerfiles("src/*.hpp")

    add_includedirs("src", { public = true })

    if is_mode("debug") then
        add_defines("DEBUG")
    end

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
        add_packages("entt", "gtest", "glm", "fmt", "spdlog")
        add_links("gtest")
        add_tests("default")
        
        add_deps("EngineSquaredCore")
        
        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end

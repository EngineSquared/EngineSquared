add_rules("mode.debug", "mode.release")

add_requires("entt", "fmt", "spdlog", "gtest")

includes("../utils/log/xmake.lua")
includes("../utils/function-container/xmake.lua")

target("EngineSquaredCore")
    set_kind("static")
    set_languages("cxx20")
    set_policy("build.warning", true)
    add_packages("entt", "spdlog", "fmt")
    add_deps("UtilsLog")
    add_deps("UtilsFunctionContainer")

    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src", { public = true })
    add_includedirs("src/entity", { public = true })
    add_includedirs("src/core", { public = true })
    add_includedirs("src/scheduler", { public = true })
    add_includedirs("src/system", { public = true })
    add_includedirs("src/plugin", { public = true })
    add_includedirs("../utils/log/src", { public = true })

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
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end
        set_default(false)
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

add_rules("mode.debug", "mode.release")
add_requires("spdlog")
set_languages("cxx20")

target("UtilsString")
    set_kind("static")
    add_packages("spdlog")
    
    add_files("src/**.cpp")
    add_headerfiles("src/**.hpp", { public = true })
    add_includedirs("src/", {public = true})

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

        add_packages("gtest")
        add_deps("UtilsString")
        
        add_files(file)
        add_files("tests/main.cpp")
        if is_mode("debug") then
            add_defines("DEBUG")
        end
    ::continue::
end
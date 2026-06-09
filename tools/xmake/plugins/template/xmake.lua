includes("../../engine/xmake.lua")

local required_packages = {
    "entt",
    "spdlog",
    "fmt",
}

local plugin_name = "PluginName"

local target_dependencies = {
    "EngineSquaredCore",
}

target(plugin_name)
    set_kind("static")
    set_group(PLUGINS_GROUP_NAME)
    set_languages("cxx20")

    add_packages(required_packages)

    add_deps(target_dependencies)

    add_files("src/**.cpp")

    for _, file in ipairs(os.filedirs("src/*")) do
        if os.isdir(file) then
            add_headerfiles("src/(" .. path.filename(file) .. "/**.hpp)")
        end
    end
    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})

for _, file in ipairs(os.files("tests/**.cpp")) do
    local name = path.basename(file)
    if name == "main" then
        goto continue
    end
    if path.filename(path.directory(file)) == "utils" then
        goto continue
    end
    target(name)
        set_group(TEST_GROUP_NAME)
        set_kind("binary")
        set_default(false)
        if is_plat("linux") then
            add_cxxflags("--coverage", "-fprofile-arcs", "-ftest-coverage", {force = true})
            add_ldflags("--coverage")
        end

        set_languages("cxx20")
        add_links("gtest")
        add_tests("default")
        add_packages(required_packages, "gtest")

        add_deps(plugin_name)
        add_files("tests/main.cpp")
        add_files(file)
        add_includedirs("tests", {public = true})
    ::continue::
end

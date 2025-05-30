add_rules("mode.debug", "mode.release")
add_requires("glfw >=3.4")

target("UtilsMock")
    set_kind("static")
    set_default(false)
    set_group(UTILS_GROUP_NAME)
    set_languages("cxx20")
    set_policy("build.warning", true)

    if is_mode("debug") then
        add_defines("ES_DEBUG")
        set_symbols("debug")
        set_optimize("none")
    elseif is_mode("release") then
        set_optimize("fastest")
    end

    add_files("src/**.cpp")
    add_packages("glfw >=3.4")

    if is_plat("windows") then
        add_ldflags("/FORCE:MULTIPLE", {force = true})
    end

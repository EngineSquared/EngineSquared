add_rules("mode.debug", "mode.release")
add_requires("spdlog", "fmt")
set_languages("cxx20")

target("UtilsLog")
    set_kind("headeronly")
    set_group(UTILS_GROUP_NAME)

    add_packages("spdlog", "fmt")

    add_includedirs("src/", {public = true})
    add_headerfiles("src/**.hpp", { public = true })

    add_installfiles("src/**.hpp", { prefixdir = "include" })


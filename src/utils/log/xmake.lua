add_rules("mode.debug", "mode.release")
add_requires("spdlog", "fmt")
set_languages("cxx20")

target("UtilsLog")
    set_kind("static")
    add_packages("spdlog", "fmt")

    add_includedirs("src/", {public = true})

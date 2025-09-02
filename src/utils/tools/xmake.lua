add_rules("mode.debug", "mode.release")
add_requires("gtest", {optional = true})

target("UtilsTools")
    set_kind("headeronly")
    set_group(UTILS_GROUP_NAME)
    set_languages("cxx20")

    add_headerfiles("src/**.h", { public = true })
    add_includedirs("src/", {public = true})

target("UtilsTools")
    set_kind("headeronly")
    set_group(UTILS_GROUP_NAME)
    set_languages("cxx20")

    add_headerfiles("src/(*.hpp)")

    add_includedirs("src/", {public = true})

target("UtilsLog")
    set_kind("headeronly")
    add_packages("spdlog", "fmt")
    set_group(UTILS_GROUP_NAME)

    add_headerfiles("src/(*.hpp)")

    add_includedirs("src", {public = true})


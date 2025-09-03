add_requires("spdlog", "fmt")

target("UtilsLog")
    set_kind("headeronly")
    set_group(UTILS_GROUP_NAME)

    add_packages("spdlog", "fmt")

    add_headerfiles("src/*.hpp")
    add_includedirs("src", {public = true})


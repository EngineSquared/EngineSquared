add_rules("mode.debug", "mode.release")
add_requires("spdlog")
set_languages("cxx20")

target("UtilsLog")
    set_kind("static")
    add_packages("spdlog")
    
    add_files("src/**.cpp")
    add_includedirs("src/", {public = true})
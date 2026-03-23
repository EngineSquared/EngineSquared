includes("../../tools/xmake/library.lua")

target("CLib")
    set_kind("shared")
    add_files("src/test.c")
    add_rules("library")

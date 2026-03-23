includes("../../tools/xmake/library.lua")

target("CppLib")
    set_kind("shared")
    add_files("src/test.cpp")
    add_rules("library")

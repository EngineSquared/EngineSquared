add_requires("zig 0.15.2")

includes("../../tools/xmake/library.lua")

target("ZigLib")
    set_kind("shared")
    add_files("src/test.zig")
    add_rules("library")
    set_toolchains("@zig")

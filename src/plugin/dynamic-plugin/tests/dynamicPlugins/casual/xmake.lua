includes("../../../../../engine/xmake.lua")
includes("./../../../tools/xmake/library.lua")
local PLUGIN_NAME = "CasualPlugin"
target(PLUGIN_NAME)
    set_kind("shared")
    add_rules("library")
    add_deps("EngineSquaredCore")

    add_files("src/CasualPlugin.cpp")

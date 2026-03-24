--!A cross-platform build utility based on Lua
--
-- Licensed under the Apache License, Version 2.0 (the "License");
-- you may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     http://www.apache.org/licenses/LICENSE-2.0
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2015-present, Xmake Open Source Community.
--
-- @author      ruki
-- @file        install.lua
--

-- imports
import("core.project.config")

-- get dotnet program
function _get_dotnet(target)
    return target:tool("cs") or "dotnet"
end

-- get build configuration from mode
function _get_configuration()
    local mode = config.mode() or "release"
    if mode:lower() == "debug" then
        return "Debug"
    end
    return "Release"
end

-- get output directory based on target kind
-- on windows, shared libraries (dll) should also go to bindir
function _get_outputdir(target)
    if target:is_binary() or (target:is_shared() and target:is_plat("windows", "mingw")) then
        return target:bindir()
    else
        return target:libdir()
    end
end

-- install csharp target using dotnet publish
function main(target)
    local installdir = target:installdir()
    if not installdir then
        return
    end

    -- get output directory based on target kind
    local outputdir = _get_outputdir(target)
    if not outputdir then
        return
    end

    -- run dotnet publish
    local csprojfile = target:data("csharp.csproj")
    local argv = {"publish"}
    if csprojfile then
        table.insert(argv, csprojfile)
    end
    table.join2(argv, {"--nologo",
        "--configuration", _get_configuration(),
        "--output", outputdir})
    local dotnet = _get_dotnet(target)
    os.vrunv(dotnet, argv)

    -- install extra files (add_installfiles)
    local srcfiles, dstfiles = target:installfiles(installdir)
    if srcfiles and dstfiles then
        for idx, srcfile in ipairs(srcfiles) do
            os.vcp(srcfile, dstfiles[idx])
        end
    end
end

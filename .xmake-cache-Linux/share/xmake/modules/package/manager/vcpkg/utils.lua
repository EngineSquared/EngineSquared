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
-- @file        utils.lua
--

-- check if a package (with optional features) is installed for the given triplet
-- e.g. is_installed(vcpkg, "curl", "x64-windows-static-md", opt)
--      is_installed(vcpkg, "curl[mbedtls]", "x64-windows-static-md", opt)
--
-- @see https://github.com/xmake-io/xmake/issues/7388
--

function need_manifest(opt)
    local require_version = opt.require_version
    if require_version ~= nil and require_version ~= "latest" then
        return true
    end
    local configs = opt.configs
    if configs and (configs.features or configs.default_features == false or configs.baseline) then
        return true
    end
end

function is_installed(vcpkg, name, triplet, opt)
    local argv = {"list", name .. ":" .. triplet, "--x-full-desc"}
    local manifest_mode = need_manifest(opt)

    -- pass feature flags to depend-info when in manifest mode, otherwise depend-info will not show the complete dependency tree with features
    if manifest_mode then
        table.insert(argv, 1, "--feature-flags=versions")
    end

    local listinfo = try { function ()
        return os.iorunv(vcpkg, argv, manifest_mode and {curdir = opt.installdir} or nil)
    end}
    if listinfo then
        local exact_prefix = name .. ":" .. triplet
        for _, line in ipairs(listinfo:split("\n", {plain = true})) do
            local first = line:split("%s")[1]
            if first == exact_prefix then
                return true
            end
        end
    end
    return false
end

-- check if all required features are installed
-- e.g. has_installed_features(vcpkg, "curl", "x64-windows-static-md", {"openssl", "mbedtls"})
--
-- @see https://github.com/xmake-io/xmake/issues/7388
--
function has_installed_features(vcpkg, name, triplet, required_features, opt)
    for _, feature in ipairs(required_features) do
        if not is_installed(vcpkg, name .. "[" .. feature .. "]", triplet, opt) then
            return false
        end
    end
    return true
end

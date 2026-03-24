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
-- @author      JassJam
-- @file        properties.lua
--

-- check if target has multi-target frameworks set
function _has_target_frameworks(context)
    return #table.wrap(context.target:values("csharp.target_frameworks")) > 0
end

-- get the first element if value is a table
function _first(value)
    if type(value) == "table" then
        return value[1]
    end
    return value
end

-- get single value from target:values()
function _get_target_value(target, name)
    return _first(target:values(name))
end

-- get default target framework from dotnet toolchain sdk version, e.g. "net8.0"
function _get_default_target_framework(context)
    local major
    local toolchain = context.target:toolchain("dotnet")
    if toolchain then
        local sdkver = toolchain:config("sdkver")
        if sdkver then
            major = tonumber(tostring(sdkver):match("^(%d+)"))
        end
    end
    return "net" .. tostring(major or 8) .. ".0"
end

-- resolve target framework from user config or auto-detect from dotnet sdk
function _resolve_target_framework(context)
    local target_framework = _get_target_value(context.target, "csharp.target_framework")
    if target_framework ~= nil and #tostring(target_framework) > 0 then
        return target_framework
    end
    return _get_default_target_framework(context)
end

-- resolve assembly name from target basename
function _resolve_assembly_name(context)
    local basename = context.target:basename()
    if basename ~= nil and #tostring(basename) > 0 then
        return basename
    end
end


-- resolve runtime identifier from user config or auto-detect from target plat/arch
-- e.g. "osx-x64", "osx-arm64", "linux-x64", "win-x64"
-- register a single-value csharp.* property entry
function _register_property(register, suffix, xml, default, extra)
    local entry = table.join({
        kind = "property",
        xml = xml,
        lua_key = "csharp." .. suffix,
        default = default
    }, extra or {})
    register(entry)
end

-- register a list-value csharp.* property entry (semicolon-joined)
function _register_list_property(register, suffix, xml, extra)
    local entry = table.join({
        kind = "property",
        xml = xml,
        lua_key = "csharp." .. suffix,
        value_type = "list",
        sep = ";"
    }, extra or {})
    register(entry)
end

-- register all csharp property and project attribute entries for csproj generation
function main()
    local entries = {}
    function register(entry)
        table.insert(entries, entry)
    end

    register({kind = "project_attribute", attr = "Sdk", lua_key = "csharp.sdk", default = "Microsoft.NET.Sdk"})
    register({kind = "property", xml = "OutputType", resolve = function (context)
        return context.target:is_binary() and "Exe" or "Library"
    end})
    _register_list_property(register, "target_frameworks", "TargetFrameworks", {when = _has_target_frameworks})
    register({kind = "property", xml = "TargetFramework", resolve = _resolve_target_framework, when = function (context)
        return not _has_target_frameworks(context)
    end})

    _register_property(register, "implicit_usings", "ImplicitUsings", "enable")
    _register_property(register, "nullable", "Nullable", "enable")
    _register_property(register, "lang_version", "LangVersion")
    _register_property(register, "enable_default_compile_items", "EnableDefaultCompileItems", "false")
    _register_property(register, "enable_default_embedded_resource_items", "EnableDefaultEmbeddedResourceItems")
    _register_property(register, "enable_default_none_items", "EnableDefaultNoneItems")
    _register_property(register, "root_namespace", "RootNamespace")
    register({kind = "property", xml = "AssemblyName", resolve = _resolve_assembly_name})
    _register_property(register, "generate_assembly_info", "GenerateAssemblyInfo")
    _register_property(register, "deterministic", "Deterministic")
    _register_property(register, "prefer_32bit", "Prefer32Bit")
    _register_property(register, "allow_unsafe_blocks", "AllowUnsafeBlocks")
    _register_property(register, "check_for_overflow_underflow", "CheckForOverflowUnderflow")
    _register_property(register, "analysis_level", "AnalysisLevel")
    _register_property(register, "enable_net_analyzers", "EnableNETAnalyzers")
    _register_property(register, "enforce_code_style_in_build", "EnforceCodeStyleInBuild")
    _register_list_property(register, "warnings_as_errors", "WarningsAsErrors")
    _register_list_property(register, "warnings_not_as_errors", "WarningsNotAsErrors")
    _register_property(register, "error_log", "ErrorLog")
    _register_property(register, "generate_documentation_file", "GenerateDocumentationFile")
    _register_property(register, "documentation_file", "DocumentationFile")

    _register_property(register, "runtime_identifier", "RuntimeIdentifier")
    _register_list_property(register, "runtime_identifiers", "RuntimeIdentifiers")
    _register_property(register, "self_contained", "SelfContained")
    _register_property(register, "use_app_host", "UseAppHost")
    _register_property(register, "roll_forward", "RollForward")
    _register_property(register, "publish_single_file", "PublishSingleFile")
    _register_property(register, "publish_trimmed", "PublishTrimmed")
    _register_property(register, "trim_mode", "TrimMode")
    _register_property(register, "publish_ready_to_run", "PublishReadyToRun")
    _register_property(register, "invariant_globalization", "InvariantGlobalization")
    _register_property(register, "include_native_libraries_for_self_extract", "IncludeNativeLibrariesForSelfExtract")
    _register_property(register, "enable_compression_in_single_file", "EnableCompressionInSingleFile")
    _register_property(register, "publish_aot", "PublishAot")
    _register_property(register, "strip_symbols", "StripSymbols")
    _register_property(register, "enable_trim_analyzer", "EnableTrimAnalyzer")
    _register_property(register, "json_serializer_is_reflection_enabled_by_default", "JsonSerializerIsReflectionEnabledByDefault")
    _register_list_property(register, "satellite_resource_languages", "SatelliteResourceLanguages")

    _register_property(register, "version", "Version")
    _register_property(register, "assembly_version", "AssemblyVersion")
    _register_property(register, "file_version", "FileVersion")
    _register_property(register, "informational_version", "InformationalVersion")
    _register_property(register, "package_id", "PackageId")
    _register_property(register, "authors", "Authors")
    _register_property(register, "company", "Company")
    _register_property(register, "product", "Product")
    _register_property(register, "description", "Description")
    _register_property(register, "copyright", "Copyright")
    _register_property(register, "repository_url", "RepositoryUrl")
    _register_property(register, "repository_type", "RepositoryType")
    _register_property(register, "package_license_expression", "PackageLicenseExpression")
    _register_property(register, "package_project_url", "PackageProjectUrl")
    _register_property(register, "neutral_language", "NeutralLanguage")
    _register_property(register, "enable_preview_features", "EnablePreviewFeatures")

    _register_property(register, "generate_runtime_configuration_files", "GenerateRuntimeConfigurationFiles")
    _register_property(register, "copy_local_lock_file_assemblies", "CopyLocalLockFileAssemblies")
    _register_property(register, "append_target_framework_to_output_path", "AppendTargetFrameworkToOutputPath", "false")
    _register_property(register, "append_runtime_identifier_to_output_path", "AppendRuntimeIdentifierToOutputPath", "false")
    _register_property(register, "produce_reference_assembly", "ProduceReferenceAssembly")
    _register_property(register, "disable_implicit_framework_references", "DisableImplicitFrameworkReferences")
    _register_property(register, "generate_target_framework_attribute", "GenerateTargetFrameworkAttribute")
    return entries
end

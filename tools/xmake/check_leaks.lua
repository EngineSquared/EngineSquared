includes(os.scriptdir() .. "/groups.lua")

local TestGroupName = TEST_GROUP_NAME

local function check_leaks_macos(targets, leaks_tool, project, os, verbose)
    local failing_targets = {}
    for _, target in pairs(targets) do
        local target_name = target:name()
        local bin_path = path.join(os.projectdir(), target:targetdir(), "debug", target:filename())

        if not os.isfile(bin_path) then
            print("Executable not found for target: " .. target_name)
            goto continue
        end

        print("Running leaks check on: " .. bin_path)
        local params = {"--atExit", "--", bin_path}
        local options = {try = true}
        if not verbose then
            options.stdout = os.nuldev()
            options.stderr = os.nuldev()
        end
        local return_value = os.execv(leaks_tool, params, options)
        if return_value ~= 0 then
            table.insert(failing_targets, target_name)
        end
        ::continue::
    end
    return failing_targets
end

task("check_leaks")
    on_run(function ()
        import("core.base.option")
        import("core.project.project")
        import("core.project.config")
        import("lib.detect.find_program")

        local wanted_target = option.get("targets") or {}
        local targets = {}
        if #wanted_target == 0 then
            for _, target in pairs(project:targets()) do
                local name = target:name()
                local group = target:info().group
                local kind = target:kind()
                if kind == "binary" and group == TestGroupName then
                    table.insert(targets, target)
                end
            end
        else
            for _, target in pairs(project:targets()) do
                local name = target:name()
                local group = target:info().group
                local kind = target:kind()
                if kind == "binary" and group == TestGroupName then
                    for _, wanted in ipairs(wanted_target) do
                        if name == wanted then
                            table.insert(targets, target)
                            break
                        end
                    end
                end
            end
        end
        local failing_targets = {}
        if os.host() == "macosx" then
            local leaks_tool = find_program("leaks", { check = "--help" })
            if not leaks_tool then
                print("leaks tool not found!")
                return
            end
            failing_targets = check_leaks_macos(targets, leaks_tool, project, os, option.get("verbose"))
        else
            print("Memory leak checking is currently only implemented for macOS.")
        end
        if #failing_targets > 0 then
            print("Targets with memory leaks found:")
            for _, target in ipairs(failing_targets) do
                print("  - " .. target)
            end
        else
            print("No memory leaks found.")
        end
    end)

    set_menu {
        usage = "xmake check_leaks",
        description = "Check for memory leaks",
        options = {
            {nil, "targets", "vs", nil, "Targets to check for leaks (default: all test targets)"}
        }
    }


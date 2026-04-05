includes(os.scriptdir() .. "/groups.lua")

local TestGroupName = TEST_GROUP_NAME

local function check_leaks_macos(targets, leaks_tool, project, os)
    local failing_targets = {}
    for _, target in pairs(targets) do
        local target_name = target:name()
        local bin_path = path.join(os.projectdir(), target:targetdir(), "debug", target:filename())

        print(bin_path)
        if not os.isfile(bin_path) then
            print("Executable not found for target: " .. target_name)
            goto continue
        end

        print("Running leaks check on: " .. bin_path)
        local return_value = os.execv(leaks_tool, {"--atExit", "--", bin_path}, {try = true, stdout = os.nuldev(), stderr = os.nuldev()})
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

        local targets = option.get("targets") or {}
        if #targets == 0 then
            -- If no targets specified, check all test targets
            targets = {}
            for _, target in pairs(project:targets()) do
                local name = target:name()
                local group = target:info().group
                local kind = target:kind()
                if kind == "binary" and group == TestGroupName then
                    table.insert(targets, target)
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
            failing_targets = check_leaks_macos(targets, leaks_tool, project, os)
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


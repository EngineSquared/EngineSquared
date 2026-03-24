task("format")
    on_run(function ()
        import("core.base.option")
        import("lib.detect.find_program")

        local tool = find_program("clang-format")
        if not tool then
            raise("clang-format not found!")
        end

        local files = option.get("files") or {"src/**/*.cpp", "src/**/*.hpp", "src/**/*.inl", "examples/**/*.cpp", "examples/**/*.hpp", "examples/**/*.inl"}

        local projectdir = "$(projectdir)"



        local flags = {"--style=file:./.clang-format"}
        -- local flags = {}

        if option.get("check") then
            flags = table.join(flags, {"--Werror", "--dry-run"})
        end

        for _, pattern in ipairs(files) do
            local filelist = os.files(pattern)
            for _, file in ipairs(filelist) do
                local params = table.join(flags, {"-i", file})
                os.execv(tool, params)
                if not option.get("check") then
                    print("Formatted: " .. file)
                end
            end
        end

        os.execv(tool, {"--help"})
    end)

    set_menu {
        usage = "xmake format [options]",
        description = "Format source code using clang-format",
        options = {
            {'f', "files", "vs", nil, "File patterns to format"},
            {'c', "check", "k", nil, "Check if files are formatted without making changes"}
        }
    }

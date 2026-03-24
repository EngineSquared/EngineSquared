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

        local flags = {"--style=file:.clang-format", "--verbose"}

        for _, pattern in ipairs(files) do
            local filelist = os.files(pattern)
            for _, file in ipairs(filelist) do
                if option.get("check") then
                    local params = table.join(flags, {file})
                    local outpath = os.tmpfile()
                    os.execv(tool, params, {stdout = outpath})
                    local out = io.readfile(outpath)
                    os.rm(outpath)
                    local ref = io.readfile(file)
                    if out ~= ref then
                        os.execv(tool, params)
                        local a = table.join(flags, {"--Werror", "--dry-run", file})
                        os.execv(tool, a)
                    end
                else
                    local params = table.join(flags, {"-i", file})
                    os.execv(tool, params)
                end
            end
        end
    end)

    set_menu {
        usage = "xmake format [options]",
        description = "Format source code using clang-format",
        options = {
            {'f', "files", "vs", nil, "File patterns to format"},
            {'c', "check", "k", nil, "Check if files are formatted without making changes"}
        }
    }

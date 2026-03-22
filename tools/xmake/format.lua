task("format")
    on_run(function ()
        import("core.base.option")
        import("lib.detect.find_tool")

        local tool = find_tool("clang-format")
        if not tool then
            raise("clang-format not found!")
        end

        local files = option.get("files") or {"**/*.cpp", "**/*.hpp", "**/*.inl"}
        for _, pattern in ipairs(files) do
            local filelist = os.files(pattern)
            for _, file in ipairs(filelist) do
                os.execv(tool.program, {"-i", file})
                print("Formatting file:", file)
            end
        end
    end)

    set_menu {
        usage = "xmake format [options]",
        description = "Format source code using clang-format",
        options = {
            {'f', "files", "vs", nil, "File patterns to format"}
        }
    }

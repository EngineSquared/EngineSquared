rule("library")
    after_build(function (target)
        if (not target:is_library() or not target:is_shared()) then
            cprint("${bright yellow}warning: Target \"" .. target:name() .. "\" is not a shared library, skipping post-build steps. Consider add 'rule(\"shared\")'${clear}")
            return
        end

        local target_name = target:name()
        local target_file_path = path(target:targetfile()):absolute():rawstr()

        import("core.base.json")

        local libs_db_file_path = path.join(target:scriptdir(), "libs.json")

        if (not os.exists(libs_db_file_path)) then
            json.savefile(libs_db_file_path, {})
        end

        local libs_db = json.loadfile(libs_db_file_path)

        libs_db[target_name] = target_file_path

        json.savefile(libs_db_file_path, libs_db)
    end)

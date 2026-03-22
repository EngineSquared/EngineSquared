add_rules("mode.debug", "mode.release")

rule("library")
    after_build(function (target)
        if (not target:is_library() or not target:is_shared()) then
            return
        end

        local target_name = target:name()
        local target_file_path = path(target:targetfile()):absolute():rawstr()

        import("core.base.json")

        local libs_db_file_path = "libs.json"

        if (not os.exists(libs_db_file_path)) then
            json.savefile(libs_db_file_path, {})
        end

        local libs_db = json.loadfile(libs_db_file_path)

        libs_db[target_name] = target_file_path

        json.savefile(libs_db_file_path, libs_db)
    end)

target("RustLib")
    set_kind("shared")
    add_files("src/test.rs")
    add_rules("library")
    set_values("rust.cratetype", "cdylib")
    after_load(function (target)
        target:set("shflags", "--crate-type=cdylib", {force=true})
    end)

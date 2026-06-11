task("plugin")
    on_run(function ()
        import("core.base.option")
        local verify = option.get("verify")
        local create = option.get("create")
        if verify and create then
            print("Please specify only one action.")
            return
        end
        if verify then
            local error = false
            local xmake_template_directory = path.join(os.scriptdir(), "plugins", "template")
            local plugin_directories = os.filedirs(path.join(os.curdir(), "src", "plugin", "*"))
            
            for _, plugin_directory in ipairs(plugin_directories) do
                local plugin_name = path.filename(plugin_directory)
                local plugin_xmake_file = path.join(plugin_directory, "xmake.lua")
                local plugin_src_directories = os.dirs(path.join(plugin_directory, "src", "*"))
                local plugin_tests_directory = path.join(plugin_directory, "tests")
                local main_test_file = path.join(plugin_tests_directory, "main.cpp")
                -- CHECK: xmake file exists in plugin directory
                if not os.isfile(plugin_xmake_file) then
                    print("xmake.lua file is missing in plugin '" .. plugin_directory .. "'.")
                    error = true
                end
                -- CHECK: all source directories are valid
                for _, src_directory in ipairs(plugin_src_directories) do
                    local src_name = path.filename(src_directory)
                    local template_src_directory = path.join(xmake_template_directory, "src", src_name)
                    if not os.isdir(template_src_directory) then
                        print("\"" .. src_name .. "\" is not a valid source directory for plugin '" .. plugin_directory .. "'.")
                        error = true
                    end
                end
                -- CHECK: main.cpp test file exists in tests directory
                if os.isdir(plugin_tests_directory) and not os.isfile(main_test_file) then
                    print("\"main.cpp\" test file is missing in the tests directory of plugin '" .. plugin_directory .. "'.")
                    error = true
                end
            end
            if error then
                raise("One or more issues found in the plugins.")
            else
                print("All plugins are well structured.")
            end
        elseif create then
            local plugin_name = "template"
            local plugins_directory = path.join(os.curdir(), "src", "plugin")
            local plugin_directory = path.join(plugins_directory, plugin_name)
            local template_directory = path.join(os.scriptdir(), "plugins", plugin_name)
            if os.isdir(plugin_directory) then
                raise("Plugin '" .. plugin_name .. "' already exists.")
            end
            os.cp(template_directory, plugins_directory)
            print("Plugin '" .. plugin_name .. "' has been created at: " .. plugin_directory)
        else
            raise("No action specified.")
        end
        
    end)

    set_menu {
        usage = "xmake plugin [options]",
        description = "Do something about plugins",
        options = {
            {'v', "verify", "k", nil, "Verify if all plugins are well structured"},
            {'c', "create", "k", nil, "Create a new plugin"}
        }
    }

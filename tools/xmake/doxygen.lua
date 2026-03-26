task("run_doxygen")
    on_run(function ()
        import("core.project.project")
        import("devel.git")

        local doxygen_package = project.required_package("doxygen")
        if not doxygen_package then
            raise("Doxygen package not found. Please install it first. By running `xmake q -y`")
        end

        local doxygen = path.join(doxygen_package:installdir(), "bin", "doxygen")

        if not os.isdir(path.join(os.projectdir(), "docs", "doxygen", "doxygen-awesome-css")) then
            print("Cloning doxygen-awesome-css repository...")
            git.clone("https://github.com/jothepro/doxygen-awesome-css.git", {
                branch = "v2.4.2",
                outputdir = path.join(os.projectdir(), "docs", "doxygen", "doxygen-awesome-css")
            })
        else
            print("Updating doxygen-awesome-css repository...")
            git.pull({
                repodir = path.join(os.projectdir(), "docs", "doxygen", "doxygen-awesome-css"),
                branch = "v2.4.2"
            })
        end

        os.run(doxygen .. " " .. path.join(os.projectdir(), "docs", "doxygen", "Doxyfile.cfg"))
    end)

    set_menu {
        usage = "xmake run_doxygen",
        description = "Generate documentation using Doxygen"
    }


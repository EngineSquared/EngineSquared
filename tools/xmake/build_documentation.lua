-- IMPORTANT: if you want to update the doxygen version, please also update the version in .github/workflows/deploy_doxygen_page.yml
local doxygen_version = "669aeeefca743c148e2d935b3d3c69535c7491e6"

add_requires("doxygen " .. doxygen_version, { debug = is_mode("debug") })

task("build_documentation")
    on_run(function ()
        import("core.project.project")
        import("devel.git")

        local doxygen_package = project.required_package("doxygen")
        if not doxygen_package or not doxygen_package:installdir() then
            raise("Doxygen package not found. Please install it first. By running 'xmake require -yvD \"doxygen " .. doxygen_version .. "\"'.")
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

        os.exec(doxygen .. " " .. path.join(os.projectdir(), "docs", "doxygen", "Doxyfile.cfg"))
    end)

    set_menu {
        usage = "xmake build_documentation",
        description = "Generate code documentation"
    }


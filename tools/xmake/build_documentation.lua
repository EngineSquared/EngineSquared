-- IMPORTANT: if you want to update the doxygen version, please also update the version in .github/workflows/deploy_doxygen_page.yml
local doxygen_version = "669aeeefca743c148e2d935b3d3c69535c7491e6"

add_requires("doxygen " .. doxygen_version, { debug = is_mode("debug"), optional = true })

task("build_documentation")
    on_run(function ()
        import("core.base.option")
        import("core.project.project")
        import("devel.git")

        local doxygen_package = project.required_package("doxygen")
        if not doxygen_package then
            raise("Doxygen package not found. Please install it first. By running:\n\nxmake require -yvD \"doxygen " .. doxygen_version .. "\"\n")
        end

        local doxygen = path.join(doxygen_package:installdir(), "bin", "doxygen")

        local docs_doxygen = path.join(os.projectdir(), "docs", "doxygen")

        local doxygen_theme_path = path.join(docs_doxygen, "doxygen-awesome-css")
        if not os.isdir(doxygen_theme_path) then
            print("Cloning doxygen-awesome-css repository...")
            git.clone("https://github.com/jothepro/doxygen-awesome-css.git", {
                branch = "v2.4.2",
                outputdir = doxygen_theme_path
            })
        end

        os.execv(doxygen, {path.join(docs_doxygen, "Doxyfile.cfg") })

        if option.get("open") then
            local index_path = path.join(docs_doxygen, "output", "html", "index.html")
            if os.isfile(index_path) then
                os.execv("open", {index_path})
            else
                print("Documentation generated, but index.html not found at: " .. index_path)
            end
        end
    end)

    set_menu {
        usage = "xmake build_documentation",
        description = "Generate code documentation",
        options = {
            {'o', "open", "k", false, "Open generated documentation after building"}
        }
    }


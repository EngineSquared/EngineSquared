package("enginesquared")
    set_description("The enginesquared package")

    add_deps("enginesquaredcore", "plugincamera", "plugincolors", "plugininput", "pluginmath", "pluginobject", "pluginphysics", "pluginscene", "pluginsound", "pluginui", "pluginopengl", "pluginwindow", "pluginrelationship", "pluginnativescripting", "pluginrenderingpipeline", "utilslog", "utilstools")

    on_load(function (package)
        package:set("installdir", path.join(os.scriptdir(), package:plat(), package:arch(), package:mode()))
    end)


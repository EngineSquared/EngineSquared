# Graphic usage

## How to run / debug on vscode

1. Remove `includes("../../xmake.lua")` from `examples/graphic_usage/xmake.lua`

2. Add the following option inside root xmake:
```lua
option("with_graphic_usage_example", {default = false, description = "Enable Graphic Usage Example"})

if has_config("with_graphic_usage_example") then
    includes("examples/graphic_usage/xmake.lua")
end
```

3. Add the following option inside xmake settings: `--with_graphic_usage_example=y`

4. Switch the mode to force the update of the setup. (debug to release or release to debug)

5. You should now see `GraphicUsage` target to compile, run, debug, etc.

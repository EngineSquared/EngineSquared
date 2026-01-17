# SoftBody Usage

## How to run / debug in VS Code

1. Add the following option inside root xmake:
```lua
option("with_softbody_usage_example", {default = false, description = "Enable SoftBody Usage Example"})
if has_config("with_softbody_usage_example") then
    includes("examples/softbody_usage/xmake.lua")
end
```

2. Add the following option inside xmake settings: `--with_softbody_usage_example=y`

3. Switch the mode to force the update of the setup. (debug to release or release to debug)

4. You should now see `SoftBodyUsage` target to compile, run, debug, etc.

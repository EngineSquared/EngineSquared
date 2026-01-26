# Sound Usage

## How to run / debug on vscode

1. Remove `includes("../../xmake.lua")` from `examples/sound_usage/xmake.lua`

2. Add the following option inside root xmake:
```lua
option("with_sound_usage_example", {default = false, description = "Enable Sound Usage Example"})

if has_config("with_sound_usage_example") then
    includes("examples/sound_usage/xmake.lua")
end
```

3. Add the following option inside xmake settings: `--with_sound_usage_example=y`

4. Switch the mode to force the update of the setup. (debug to release or release to debug)

5. You should now see `SoundUsage` target to compile, run, debug, etc.

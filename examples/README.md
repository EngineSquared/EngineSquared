# Examples

This directory contains example projects that demonstrate how to use EngineSquared. Each example is a self-contained project that can be built and run independently.

## Building and Running Examples

To build and run an example, be at the root of the EngineSquared repository and use the following commands, replacing `[name_of_example]` with the name of the example you want to run:

```bash
xmake f --[name_of_example]=y
xmake run -y [name_of_example_target_name]
```

for example, to run the `basic_core_usage` example, you would use:

```bash
xmake f --basic_core_usage=y
xmake run -y BasicCoreUsage
```

## Non-exhaustive list of examples

- `basic_core_usage`: A simple example demonstrating the basic usage of EngineSquared's core features.
- `event_usage`: An example showcasing how to use the event system.
- `graphic_light_usage`: An example illustrating how to use lighting features.
- `graphic_material_usage`: An example demonstrating how to use materials.
- `graphic_usage`: An example showcasing the basic graphics features.
- `graphic_usage_with_physics_usage`: An example demonstrating how to use graphics features with physics features.
- `input_usage`: An example illustrating how to handle user input.
- `nativescripting_usage`: An example showcasing how to use native scripting.
- `physics_usage`: An example showcasing the basic physics features.
- `relationships_usage`: An example demonstrating how to use relationships between entities.
- `rmlui_usage`: An example showcasing how to use RmlUI for user interfaces.
- `softbody_usage`: An example demonstrating how to use soft body physics features.
- `sound_usage`: An example showcasing how to use sound features.
- `vehicle_usage`: An example demonstrating how to use vehicle physics features.

## Some flags that you can use when building examples

- `--all_examples=[y/n]`: Add all examples to the build.
- `--[name_of_example]=[y/n]`: Add a specific example to the build, replacing `[name_of_example]` with the name of the example you want to add. For example, `--basic_core_usage=y` will add the `basic_core_usage` example to the build.
- `--executable_examples=[y/n]`: Add all examples that can be built and run in a container. This is useful for running examples in a CI environment or on platforms where you cannot run the examples directly on the host machine.

# Examples

This directory contains example projects that demonstrate how to use EngineSquared. Each example is a self-contained project that can be built and run independently.

## Building and Running Examples

To build and run an example, be at the root of the EngineSquared repository and use the following commands, replacing `[NameOfExample]` with the name of the example you want to run:

```bash
xmake f --[NameOfExample]=y
xmake run -y [NameOfExamplesTargetName]
```

for example, to run the `BasicCoreUsage` example, you would use:

```bash
xmake f --BasicCoreUsage=y
xmake run -y BasicCoreUsage
```

## Non-exhaustive list of examples

- `BasicCoreUsage`: A simple example demonstrating the basic usage of EngineSquared's core features.
- `EventUsage`: An example showcasing how to use the event system.
- `GraphicLightUsage`: An example illustrating how to use lighting features.
- `GraphicMaterialUsage`: An example demonstrating how to use materials.
- `GraphicUsage`: An example showcasing the basic graphics features.
- `GraphicUsageWithPhysicsUsage`: An example demonstrating how to use graphics features with physics features.
- `InputUsage`: An example illustrating how to handle user input.
- `NativeScriptingUsage`: An example showcasing how to use native scripting.
- `PhysicsUsage`: An example showcasing the basic physics features.
- `CharacterControllerUsage`: An example demonstrating how to use the CharacterController component.
- `RelationshipsUsage`: An example demonstrating how to use relationships between entities.
- `RmluiUsage`: An example showcasing how to use RmlUI for user interfaces.
- `SoftbodyUsage`: An example demonstrating how to use soft body physics features.
- `SoundUsage`: An example showcasing how to use sound features.
- `VehicleUsage`: An example demonstrating how to use vehicle physics features.

## Some flags that you can use when building examples

- `--AllExamples=[y/n]`: Add all examples to the build.
- `--[NameOfExample]=[y/n]`: Add a specific example to the build, replacing `[NameOfExample]` with the name of the example you want to add. For example, `--BasicCoreUsage=y` will add the `BasicCoreUsage` example to the build.
- `--ExecutableExamples=[y/n]`: Add all examples that can be built and run in a container. This is useful for running examples in a CI environment or on platforms where you cannot run the examples directly on the host machine.

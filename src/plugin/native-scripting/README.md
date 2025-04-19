# native-scripting/README.md

# Native Scripting Module

This project implements a native scripting module for the EngineSquared framework, allowing for dynamic script execution and management within the engine.

## Project Structure

- **src/**: Contains the source code for the native scripting module.
  - **component/**: Defines the `NativeScripting` component.
  - **plugin/**: Implements the `PluginNativeScripting` class for plugin integration.
  - **system/**: Contains the `ScriptingSystem` namespace for script management.
  - **utils/**: Provides the `ScriptableEntity` class for entity interaction.

- **tests/**: Contains unit tests for the native scripting functionality.
  - **main.cpp**: Entry point for the test suite.
  - **ScriptTest.cpp**: Unit tests for verifying script lifecycle.

- **xmake.lua**: Build configuration file specifying rules, dependencies, and targets.

## Setup Instructions

1. Clone the repository:
   ```
   git clone <repository-url>
   ```

2. Navigate to the project directory:
   ```
   cd native-scripting
   ```

3. Build the project using xmake:
   ```
   xmake
   ```

## Usage Guidelines

- To use the native scripting functionality, include the relevant headers in your project and register the `NativeScripting` component with your entities.
- Implement your scripts by deriving from `ScriptableEntity` and overriding the lifecycle methods (`OnCreate`, `OnUpdate`, `OnDestroy`).
- Use the `PluginNativeScripting` class to bind the scripting systems to the engine.

## Testing

Run the tests using the following command:
```
xmake test
```

This will execute all unit tests defined in the `tests` directory and report the results.
# Engine² (or Engine Squared)

Open-source physics game engine written in C++.
<br>
Epitech's 4th/5th year end project (EIP)

## Description

Engine² is a game engine that aims to provide a developer friendly and open-source alternative for 3D game development.
It is designed to provide accurate physics simulation and visually appealing graphics in a single package.

## Getting started

### Requirements

- [Xmake](https://xmake.io/#/)
- [Vulkan SDK](https://vulkan.lunarg.com/sdk/home)
- [Visual Studio (for development)](https://visualstudio.microsoft.com/)

### Compile the library

1. Run `xmake build`
2. Install required dependencies if needed (or use `xmake build -y` to install them automatically)

### Set up a development environment

1. Open the repository using [Visual Studio](https://visualstudio.microsoft.com/)
2. Execute `xmake project -k vsxmake -m "debug,release"` inside the root using a VS "Developer PowerShell"
3. Open the created VS Solution using [Visual Studio](https://visualstudio.microsoft.com/) (/vsxmake2022/EngineSquared.sln)
4. (Optional) Install Xmake VS extension
5. Build the solution

## Run unit tests

1. Run `xmake test`
2. Install required dependencies if needed (or use `xmake test -y` to install them automatically)
3. Tests will be executed individually

## Build the documentation

1. Install Doxygen using your [package manager](https://command-not-found.com/doxygen) or download it from the [official website](https://www.doxygen.nl/download.html)
2. Run `mkdir -p docs && doxygen Doxyfile.cfg` to generate the documentation using Doxygen
3. Open the generated documentation using a web browser (/docs/EngineSquared/html/index.html)

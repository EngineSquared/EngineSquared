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

## Coding style

1. Download clang-format from [here](https://releases.llvm.org/download.html) or from [github](https://github.com/llvm/llvm-project/releases/latest)
2. Add the clang-format executable to your PATH

### Apply the coding style

For bash users, you can use the following command to apply the coding style to the project:
```bash
find . -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
```

For Windows users, you can use the following command to apply the coding style to the project:
```powershell
Get-ChildItem -Recurse -Include *.cpp, *.hpp | ForEach-Object { clang-format -i $_.FullName }
```

For Visual Studio users, you can use the [ClangFormat extension](https://marketplace.visualstudio.com/items?itemName=LLVMExtensions.ClangFormat) to format the code.

## Build the documentation

1. Download Doxygen from the [official website](https://www.doxygen.nl/download.html)
2. Create a `docs` folder if it doesn't already exist and run `doxygen Doxyfile.cfg` to generate the documentation using Doxygen
3. Open the generated documentation using a web browser (/docs/EngineSquared/html/index.html)

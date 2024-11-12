# CONTRIBUTING GUIDELINES

Engine² is an Open-source physics game engine written in C++ and it's also an Epitech's 4th/5th year-end project (EIP). Contribution is welcome, encouraged, and appreciated.
It is also essential for the development of the project.

See the [Table of Contents](#table-of-contents) for different ways to help and details about how this project handles them. Please make sure to read the relevant section before making your contribution. It will make it a lot easier for us maintainers and smooth out the experience for all involved. The community looks forward to your contributions. 🎉

> [!NOTE]
> If you like the project, but just don't have time to contribute, that's fine. There are other easy ways to support the project and show your appreciation, which we would also be very happy about:
> - Star the project
> - Tweet about it
> - Refer this project in your project's readme
> - Mention the project at local meetups and tell your friends/colleagues


## Table of Contents

- [How to Contribute](#how-to-contribute)
  - [Creating an Issue](#creating-an-issue)
  - [Creating a Pull Request](#creating-a-pull-request)
- [Project Structure](#project-structure)
- [Commit naming convention](#commit-naming-convention)
  - [Commit structure](#commit-structure)
    - [Type](#type)
    - [Scope](#scope)
    - [Summary](#summary)
- [Coding Style](#coding-style)
  - [Local Coding Style](#local-coding-style)
    - [Apply the coding style](#apply-the-coding-style)
  - [Automatic Coding Style](#automatic-coding-style)
  - [Namespace convention](#namespace-convention)
  - [Header](#header)
  - [File and folder naming](#file-and-folder-naming)
- [Code Documentation](#code-documentation)
- [Testing Policies](#testing-policies)
  - [Unit tests](#unit-tests)
  - [Functional tests](#functional-tests)
- [Support and Contact](#support-and-contact)
- [Core Team](#core-team)

## How to contribute

You can contribute to the project in several ways. You can either report a bug, request a feature, or discuss the current state of the project. To do so, you can create an issue or a pull request.

### Creating an issue

Before contributing, make sure to check if there's an existing issue for the task you want to work on.

If not, you can create an issue with the following rules:

- Name the issue with a short description
- Describe the issue with a description
- Add labels to the issue

### Creating a pull request

Before contributing, make sure to check if there's an existing pull request for the task you want to work on.

When you do a pull request, you have to use the following rules:

- Name the pull request with a short description
- Describe the pull request with a description of what you did
- Add the core team as reviewers
- You have to squash your commits to one commit with the number of the issue and the short description. (It should be automatic)

If the pull request is related to an issue, you have to add the issue number in the description.

If the pull request is related to a graphical feature, you have to add a screenshot of the feature in the description or a link to a video or a link to a repository with the feature.

To meet the requirements of the project, the pull request has to be approved by the core team and have tests if applicable. (Look at [Testing Policies](#testing-policies) for more details)

## Project structure

Currently, the project contains 2 main parts: the engine squared core and plugins, respectively located in `./src/engine` and `./src/plugin`.
The core contains the minimal code to have a working engine. The plugins contain the code for the plugins that can be added to the engine. The plugins are part of the engine that is used for a specific purpose (`Math` for every utility math functions or `VkWrapper` for all the Vulkan wrapper and utility functions).

## Commit naming convention

### Commit structure

```
<type>(<scope>): <short summary>
  │       │             │
  │       │             └─⫸ Summary in present tense. Not capitalized. No period at the end.
  │       │
  │       └─⫸ Commit Scope
  │
  └─⫸ Commit Type: build|ci|docs|feat|fix|perf|refactor|test|chore
```

The `<type>` and `<summary>` fields are mandatory, the `(<scope>)` field is optional.

#### Type

Must be one of the following:

* **build** : Changes that affect the build system or external dependencies (example scopes: gulp, broccoli, npm)
* **ci** : Changes to our CI configuration files and scripts (examples: CircleCi, SauceLabs)
* **docs** : Documentation only changes
* **feat** : A new feature
* **fix** : A bug fix
* **perf** : A code change that improves performance
* **refactor** : A code change that neither fixes a bug nor adds a feature
* **test** : Adding missing tests or correcting existing tests
* **chore** : Chore changes (update .gitignore, dependencies, etc.)

#### Scope

The scope should be the name of the file, the directory or the feature involved in the commit.

Here are some examples:

* gitignore
* main
* action
* readme

#### Summary

Use the summary field to provide a succinct description of the change:

* use the imperative, present tense: "change" not "changed" nor "changes"
* don't capitalize the first letter
* no dot (.) at the end

> [!IMPORTANT]
> We a CI that check if your commit message is correct. If it's not, you will have to change it before pushing your code.

## Coding style

To have a consistent codebase, we follow a coding style. This coding style is enforced by clang-format. You can find the configuration file in the root of the project.

### Local coding style

1. Download clang-format from [here](https://releases.llvm.org/download.html) or from [Github](https://github.com/llvm/llvm-project/releases/latest)
2. Add the clang-format executable to your PATH

#### Apply the coding style

For bash users, you can use the following command to apply the coding style to the project:
```bash
find . -iname '*.hpp' -o -iname '*.cpp' | xargs clang-format -i
```

For Windows users, you can use the following command to apply the coding style to the project:
```powershell
Get-ChildItem -Recurse -Include *.cpp, *.hpp | ForEach-Object { clang-format -i $_.FullName }
```

### Automatic coding style

We also have a CI that will update the code style automatically. So don't forget to pull the latest changes before pushing your code.

### Namespace convention

As we are using C++, we have to use namespaces. The namespace should define precisely what the file is about. For example, if you have a `.hpp` that contains all the math functions, you should put it in the `math` namespace like:

```cpp
namespace ES::Plugin::Math::3D {
  // Your code here
}
```

and in the `.cpp` file, you should put it in the same namespace as:

```cpp
ReturnType ES::Plugin::Math::3D::FunctionName() {
  // Your code here
}
```

> [!WARNING]
> Don't put global namespace in the `.cpp` file like:
> ```cpp
> namespace ES::Plugin::Math::3D {
>   ReturnType FunctionName() {
>     // Your code here
>   }
> }
> ```

### Header

Each `.hpp` must contain an header guard of this form:
  
```cpp
/**************************************************************************
 * [name of the project] [version of the project]
 *
 * [name of the project] is a software package, part of the Engine².
 *
 * This file is part of the [name of the project] project that is under GPL-3.0 License.
 * Copyright © 2024 by @EngineSquared, All rights reserved.
 *
 * [name of the project] is a free software: you can redistribute it and/or modify
 * it under the terms of the GPL-3.0 License as published by the
 * Free Software Foundation. See the GPL-3.0 License for more details.
 *
 * @file [name of the file]
 * @brief [short description of the file]
 *
 * [long description of the file]
 *
 * @author @[your name]
 * @version [version of the file]
 * @date [date of the file]
 **************************************************************************/
```

You can find an example of a header guard in the [vk-wrapper/src/queueFamilies/QueueFamilies.hpp](https://github.com/EngineSquared/EngineSquared/blob/main/src/plugin/vk-wrapper/src/queueFamilies/QueueFamilies.hpp) plugin file.

### File and folder naming

Source and header files should be named with PascalCase convention like `FooBar.hpp` or `FooBar.cpp`.

Folder names should be named with dash-case convention like `./foo-bar/`.

## Code Documentation

To have a consistent codebase, we follow a documentation style. This documentation style is enforced by Doxygen. You can find the configuration file in the root of the project.

## Testing policies

### Unit tests

Library: [GTest](https://google.github.io/googletest/primer.html)

We want to have a good coverage of the codebase. We don't have a coverage goal at the moment, but maximizing coverage is a good thing.

### Functional tests

We don't force you to do functional tests, but if you can do it, it's better. You can create a little project demonstrating the feature you added.

## Support and contact

If you have any questions or need assistance, refer to the [Core Team](#core-team) or open an issue.

## Core Team

 | <a href="https://github.com/Miou-zora"><img alt="Miou-zora" src="https://avatars.githubusercontent.com/u/91665686?v=4" width="100"></a> | <a href="https://github.com/ripel2"><img alt="Ripel2" src="https://avatars.githubusercontent.com/u/70372762?v=4" width="100"></a> | <a href="https://github.com/MasterLaplace"><img alt="MasterLaplace" src="https://avatars.githubusercontent.com/u/60772783?v=4" width="100"></a> |
| --- | --- | --- |
| [Miouzora](https://github.com/Miou-zora) | [Ripel2](https://github.com/ripel2) | [Master_Laplace](https://github.com/MasterLaplace) |
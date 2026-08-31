# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Build & test commands

Build system is **xmake only** (no CMake). Dependencies are fetched automatically by `xmake f`.

```bash
xmake f -y                      # configure (release by default)
xmake f -y -m debug             # configure in debug (needed for tests/coverage)
xmake build -y                  # build the whole engine
xmake clean -a                  # full clean (do this when switching modes)
```

Tests (GoogleTest, one binary per test file, all in the `UnitTests` group):

```bash
xmake f -y -m debug && xmake test -y -v   # all tests
xmake test EntityTest                     # single test target (target name == test file basename)
xmake test -g UnitTests                   # whole test group
xmake check_leaks                         # leak check over all test targets (leaks / Valgrind / Dr. Memory by host)
xmake check_leaks EntityTest              # single target (positional; --targets is not a flag)
```

Lint / format (CI enforces both):

```bash
xmake format                    # clang-format -i over src/ and examples/
xmake format -c                 # check only, no writes (what CI runs)
xmake check -y clang.tidy --configfile=./.clang-tidy
```

CI uses **clang-format-22** / clang-tidy 22; `.clang-tidy` has `WarningsAsErrors: "*"` on `bugprone-*`.

Examples are opt-in xmake options, one per example directory name:

```bash
xmake f --BasicCoreUsage=y -y && xmake run BasicCoreUsage
xmake f --AllPrimaryExamples=y -y -m debug && xmake build -y   # everything without a .secondary marker
xmake f --ExecutableExamples=y -y -m debug && xmake run -y -v  # only dirs with a .ci_run_target marker
xmake f --AllExamples=y -y
```

An example dir gets a `.secondary` file when it needs external deps (raylib/sfml/ncurses) and should stay out of the primary CI build; `.ci_run_target` marks headless examples CI actually executes.

Docs: `xmake build_documentation [-o]` (doxygen, auto-installs; theme cloned into `docs/doxygen/doxygen-awesome-css`).

Plugin scaffolding: `xmake scaffold --create` copies `tools/xmake/plugins/template` into `src/plugin/template`; `xmake scaffold --verify` checks every plugin has `xmake.lua`, only template-approved `src/` subdirectory names, and a `tests/main.cpp` if it has tests.

## Architecture

ECS built on **EnTT**. `Engine::Core` (`src/engine/src/core/Core.hpp`) owns everything: the registry, resources, schedulers+systems, and plugins.

- **Entity** — thin wrapper over an `Engine::Id` + `Core &`. Components are plain structs holding data only. `AddTemporaryComponent` exists for one-frame components cleared by `Entity::RemoveTemporaryComponents(core)`.
- **System** — any callable taking `Engine::Core &`. Registered with `core.RegisterSystem<Scheduler>(fn...)`, or `RegisterSystemWithErrorHandler` to attach a throw callback. Systems hold no state; state lives in components or resources.
- **Resource** — a singleton-ish value stored by type in the Core (`RegisterResource` / `GetResource<T>()`), e.g. `Engine::Resource::Time`, `Physics::Resource::PhysicsManager`.
- **Scheduler** — decides when a set of systems runs. Built-ins: `Startup`, `Update` (default), `FixedTimeUpdate`, `RelativeTimeUpdate`, `Shutdown`. Ordering between schedulers is a dependency graph: `SetSchedulerBefore<A, B>()` / `SetSchedulerAfter` / `RemoveDependencyBefore|After`. `RunSystems()` executes one pass; `Run()` loops until `Stop()`.
- **Plugin** — the unit of feature packaging. Derive from `Engine::APlugin`, implement `Bind()`, and register everything there: `RequirePlugins<Other::Plugin>()` pulls dependencies, then `RegisterResource`, `RegisterScheduler`, `RegisterSystems<Scheduler>(...)`. Users call `core.AddPlugins<Physics::Plugin, Graphic::Plugin>()`. See `src/plugin/physics/src/plugin/PluginPhysics.cpp` for the canonical `Bind()`.

Layout: `src/engine` (core, target `EngineSquaredCore`), `src/plugin/<name>` (target `Plugin<Name>`), `src/utils/<name>` (target `Utils<Name>`). The root `EngineSquared` target just aggregates all plugin targets.

### Plugin internals convention

Inside `src/plugin/<name>/src/`, only these subdirectories are allowed (enforced by `xmake scaffold --verify`, mirrored from the template): `component`, `system`, `resource`, `scheduler`, `event`, `exception`, `utils`, `plugin`, plus optional extras already present in the template. Namespaces mirror the folders: `Physics::Component`, `Physics::System`, `Physics::Resource`, `Physics::Utils`, `Physics::Exception`, and the plugin class is always `<Name>::Plugin` in `src/plugin/Plugin<Name>.hpp`.

Each plugin ships an umbrella header `src/<Name>.hpp` that includes its whole public surface, and usually a `src/<Name>.pch.hpp` wired via `set_pcxxheader`. Headers are exported with `add_headerfiles("src/(<dir>/*.hpp)")` and `add_includedirs("src", {public = true})`, so cross-plugin includes are path-relative to a plugin's `src/` (e.g. `#include "plugin/APlugin.hpp"`, `#include "component/RigidBody.hpp"`).

Template-heavy code is split: declarations in `.hpp`, definitions in a `.ipp` included at the bottom of the header.

### Adding a plugin's xmake target

Copy the pattern from `tools/xmake/plugins/template/xmake.lua`: `includes("../../engine/xmake.lua")` plus any plugin deps, one static-library target in `PLUGINS_GROUP_NAME`, then a loop over `tests/**.cpp` creating one binary target per test file (skipping `main.cpp`), each in `TEST_GROUP_NAME` with `set_default(false)`, `add_tests("default")`, gtest linkage, and `--coverage` flags on Linux. Group name constants come from `tools/xmake/groups.lua`. New plugins must also be added to the `EngineSquared` target's `add_deps` list in the root `xmake.lua`.

## Testing policy

From the wiki Testing Policy page:

- Test files must be named `<Thing>Test.cpp` and live in a `tests/` folder next to the code they cover. Each file becomes its own executable target; `tests/main.cpp` only holds the gtest runner.
- Coverage objective is **90% of testable features**.
- Unit tests are expected for: entity/component manipulation, system and resource containers, system scheduling, resource load/unload, and all non-graphical, non-audio plugins (physics, scripting, scene, utils).
- Graphics and sound plugins are **exempt from unit tests** — they are covered by functional tests (an example that renders/plays and is run in CI) instead.
- Functional tests are also expected for plugin creation + binding into the Core, physics simulation accuracy, and script execution.

## Conventions

### Commits and PRs

Angular convention, CI-enforced for both commit titles and PR titles (`.github/workflows/commit_norm_check.yml`, `.github/scripts/pr_title_check.py`):

```
<type>(<scope>): <short summary>
```

`type` ∈ `build|ci|docs|feat|fix|perf|refactor|test|chore`; scope is optional and usually the plugin (`core`, `physics`, `graphic`, ...); summary is imperative present tense, lowercase first letter, no trailing period, English.

PR rules from the wiki: title identical to the commit message, squash down to a single commit, link the related issue in the description, keep every checkbox of the PR template (write "None" when a section does not apply), request the core team as reviewers, and attach a screenshot or video for anything graphical. Merge needs one core-team approval plus green CI. A PR labelled `critical` only merges with approval from the whole core team.

### Code style

C++20, LLVM-based clang-format (`.clang-format`), plus the wiki's explicit rules:

- Namespaces are `PluginName::Kind` where Kind ∈ `Component`, `System`, `Resource`, `Plugin`, `Utils`, `Exception`.
- **Files are PascalCase** (`FooBar.hpp`), **folders are dash-case** (`camera-movement/`, `native-scripting/`).
- Prefer `{}` initialisers over `()`.
- Documentation comments use `///` — never `//!` or `/** */`.
- `#pragma once`, never include guards.
- No abbreviated identifiers (`nbr`, `ctx`, `lib` → spell them out).
- `PascalCase` methods, `_camelCase` private members.

### ADRs

Architecture decisions live in `docs/decisions/`, from `00-adr-template.md`. Lifecycle: `todo` → `in-progress` → `completed` (or `abandoned`). An ADR may only move to `in-progress` once every entry in its `depends_on` is `completed`; opening the PR adds the `pr` / `pr_url` fields and the body carries `Closes #<issue>`; an abandoned ADR keeps a section explaining why.

### Repo hygiene

CI rejects stray build artifacts (`*.o`, `*.a`, `*.so`, `*.gcda`, `*.gcno`, `*~`, `#...#`, `tmp/`) anywhere in the tree.

## AI usage policy

The wiki Contributing page restricts AI use in this project: **directly generating or copy/pasting AI-written code is not allowed.** AI is accepted for applying refactors already decided (e.g. bulk renames), explaining concepts, assisting review, challenging/brainstorming a design, and generating unit tests with care. Assume any produced code has to be understood, justified and reworked by the human contributor before it lands.

## Wiki accuracy warning

The GitHub wiki (https://github.com/EngineSquared/EngineSquared/wiki, 16 pages, mostly last edited May 2025 for v0.2.0) is the source for the process rules above, but its code examples are stale — do not copy them:

- Wiki examples still use the removed `ES::` prefix (`ES::Engine::Core`, `ES::Plugin::Input::Plugin`). Current code is `Engine::Core`, `Input::Plugin`, `Physics::Component::…`.
- Its plugin list names `OpenGL`, `Math` and `Scene Management`; the repo actually ships a WebGPU `graphic` plugin, no `math` plugin, plus `event`, `sound`, `camera-movement`, `default-pipeline` and `rmlui`.
- It documents consuming the engine via `includes("../EngineSquared/xmake.lua")` + `add_deps("EngineSquared")`; the README's current path is the xrepo registry (`xrepo add-repo ...` then `xmake create -t engine-squared`).
- It points at `docs/doxygen/html/index.html`; the task writes to `docs/doxygen/output/html/index.html`.

Pages worth reading in full when relevant: Contributing, Testing Policy, and "Graphic & Default pipeline" (the only detailed rendering doc: WebGPU/wgpu-native, deferred G-buffer + 2048² PCF shadow maps, CPU components mirrored by auto-managed `GPU*` components that must not be edited by hand, and the Init/Setup/Startup → PreUpdate/Update/RenderSetup/Preparation/CommandCreation/ToGPU/Draw/Presentation scheduler chain). Physics, Window, Relationship, UI, Math and Rendering Pipeline are one-line stubs — read the headers instead.

## Learning track (in progress)

A rustlings-style exercise track for the engine core, tracked by epic **#617**. Design, constraints and rejected options: ADR [`docs/decisions/01-learning-track.md`](docs/decisions/01-learning-track.md) — read it before changing the track or its integration points.

- Ships as an installable xmake plugin, not a template: `xmake plugin --install github:EngineSquared/learn` then `xmake learn init`. Templates were rejected because `xmake create -t` cannot update a project after generation. Needs xmake 3.1.0+, above this repo's stated 3.0.x floor.
- Lives in the separate `EngineSquared/learn` repo (#619) so learners never clone the engine; the engine arrives as the `enginesquared` xrepo package with a `core_only` config (EngineSquared/xrepo#1).
- This repo gains only an ADR (#637) and a README link (#638).
- The tutor contract for AI assistants lives in `TUTOR.md` in the learn repo, referenced from `AGENTS.md` — not here, and not Claude-specific.

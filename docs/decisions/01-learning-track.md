---
# This template comes from MADR (https://adr.github.io/madr/)

issue: ADR-001
title: Progressive learning track for the engine core
branch: docs/637-learning-track-adr
status: in-progress
pr: 640
pr_url: https://github.com/EngineSquared/EngineSquared/pull/640
github_issue: 617
github_issue_url: https://github.com/EngineSquared/EngineSquared/issues/617
depends_on: []
required_by: []
---
# Progressive learning track for the engine core

## Status

accepted; the frontmatter `status: in-progress` tracks delivery of the epic, not the decision itself.

## Date

2026-08-28

## Context and Problem Statement

The engine core has no learning path. Newcomers currently have one substantial example and the test suite to learn from, while the wiki is stale enough to show APIs such as the removed `ES::` prefix. How can someone learn the core progressively without first cloning and understanding the engine repository?

This decision is part of the [progressive learning track epic](https://github.com/EngineSquared/EngineSquared/issues/617).

## Decision Drivers

The design must satisfy these six constraints:

* Learners never clone the EngineSquared repository.
* A review agent checks the tutorials as a human learner would.
* Learners may know nothing about C++, ECS, or build systems.
* The tool watches exercise files and advances automatically.
* An assisting AI asks questions instead of providing answers.
* The assisting AI is not assumed to be Claude.

## Considered Options

* An installable xmake addon in a separate learning repository
* An xmake project template
* Exercises stored in the EngineSquared repository
* A curl-pipe installer

## Decision Outcome

Chosen option: "An installable xmake addon in a separate learning repository", because it gives learners a small installation command, permits curriculum updates after initialization, and keeps the curriculum independent from the engine source tree.

The addon is installed with:

```sh
xmake addon --install github:EngineSquared/learn
```

The learning track requires xmake 3.1.0 or newer because that version introduced the manager used by the installation command. xmake 3.1.1 spells it `xmake addon` and keeps `xmake plugin` as a deprecated alias that prints a warning, so the documented command is the `addon` one and [#618](https://github.com/EngineSquared/EngineSquared/issues/618) records which spellings the supported versions accept. The addon and curriculum live in the [EngineSquared/learn repository](https://github.com/EngineSquared/learn). Learning projects consume EngineSquared through the `enginesquared` xrepo package with its `core_only` configuration, so learners receive the core without cloning or building this repository.

An xmake project template still ships in [EngineSquared/xrepo](https://github.com/EngineSquared/xrepo) as a thin fallback for people who prefer a scaffold; the project it generates only tells the learner to install the addon, so there is one source of truth for the curriculum.

Two guardrails follow from the last two decision drivers and are part of this decision:

* The runner holds an explain-back gate: a passing exercise stays pending until the learner answers its comprehension question. The check is offline string matching, so it works no matter who or what wrote the code, and it does not depend on any vendor's guardrails.
* The tutor contract lives in `TUTOR.md` in the learning repository and is referenced from `AGENTS.md`, so every agent that reads the AAIF format gets the same five-stage question ladder. Tool-specific enforcement, such as Claude Code hooks, is an addition on top of it, never the mechanism it relies on.

### Consequences

* Good, because learners can install the track without cloning the engine repository.
* Good, because the curriculum can be updated while preserving existing learner work.
* Good, because the separate repository can contain its own runner, book, tutor contract, and learner-focused QA.
* Good, because consuming the `core_only` package keeps the track focused on the ECS core.
* Bad, because the learning track can drift as the engine API changes.
* Bad, because the addon, xrepo package configuration, and separate repository create more release surfaces to maintain.
* Bad, because installing a remote addon executes code from another repository, so learners have to trust the `EngineSquared/learn` releases the same way they already trust the xrepo packages.
* The curriculum-rot risk is mitigated by a nightly job that verifies all solutions against the engine's `main` branch.

### Confirmation

The decision is confirmed when the learning track installs through xmake, initializes without an EngineSquared source clone, consumes the `core_only` package, and its nightly job passes against the engine's `main` branch. A first-contact review agent must also complete the tutorials using the defined learner personas.

## Pros and Cons of the Options

### An installable xmake addon in a separate learning repository

* Good, because one xmake command installs the learning workflow.
* Good, because addon updates can deliver new chapters and API-drift fixes after a learner initializes a project.
* Good, because EngineSquared remains a package dependency instead of becoming learner workspace content.
* Neutral, because the curriculum and engine are maintained in separate repositories.
* Bad, because this depends on xmake's addon distribution support and a compatible `core_only` package.

### An xmake project template

* Good, because it uses xmake and can generate a ready-to-use project.
* Bad, because generated projects cannot receive curriculum updates.
* Bad, because `xmake create -t` resolves templates only from global, addon, and builtin directories, which makes it unsuitable as the primary remotely maintained distribution path.

### Exercises stored in the EngineSquared repository

* Good, because exercises and engine changes could be reviewed together.
* Bad, because learners would have to clone the engine repository, violating a core constraint.
* Bad, because the engine's size and architecture would add noise to a beginner's workspace.

### A curl-pipe installer

* Good, because it can offer a short installation command on systems with curl and a compatible shell.
* Bad, because piping downloaded code into a shell is difficult for learners to inspect and trust.
* Bad, because it introduces platform-specific shell behavior despite xmake already providing an addon manager.

## More Information

The epic defines the runner, curriculum, guardrails, quality checks, and adoption work: [EngineSquared/EngineSquared#617](https://github.com/EngineSquared/EngineSquared/issues/617). The implementation belongs in [EngineSquared/learn](https://github.com/EngineSquared/learn), with the `core_only` package configuration maintained in [EngineSquared/xrepo](https://github.com/EngineSquared/xrepo).

The [addon distribution validation](https://github.com/EngineSquared/EngineSquared/issues/618) records which reference syntaxes `xmake addon --install` accepts and the minimum xmake version, on Linux, macOS and Windows.

This decision should be revisited if xmake addon distribution cannot support the required platforms or if the nightly compatibility job cannot detect curriculum drift reliably.

## Abandonment

Not abandoned.

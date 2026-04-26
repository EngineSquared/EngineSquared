# Tools

This folder consists of some tools that can be used by the contributors of the engine to help them do different work.

For now you can find 2 folders:
- `ai`: This folder contains all tools that can improve AI to do specific stuff. For example, you can find `copilot/copilot-commit-message-instructions.md` that describes the commit message template for Copilot to generate commit messages.
- `xmake`: This folder contains all tools that can be used with xmake. Most of them are xmake tasks that can be found when running `xmake -h` in the section "Tasks". If you want to have more details, you can run the task with the `-h` flag, like `xmake format -h`. Some of the other xmake files can also be used in different contexts. For example, `groups.lua` describes some target groups.

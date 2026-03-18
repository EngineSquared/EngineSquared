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
* commit must be in english

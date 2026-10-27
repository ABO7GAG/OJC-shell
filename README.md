# OJC-shell

**A small shell with a larger destination.**

OJC-shell is a lightweight, Unix-like command-line shell written in C. It began as a hands-on experiment in understanding how a shell works beneath the prompt: reading commands, parsing tokens, creating processes, connecting pipes, and communicating with the operating system.

It is also the first command-line building block in the wider **OJclicks OS** vision.

> Build the foundation. Understand every layer. Keep control.

---

## The story behind OJC-shell

OJC-shell started on **March 22, 2026** as a small C project: a place to learn by building rather than treating the terminal as a black box.

The early work focused on the essentials of a shell: executing programs, handling built-in commands, and making command input useful. As the project grew, the work moved deeper into the details that make shells difficult: token boundaries, quoting, escaping, environment variables, command history, pipelines, and redirection.

The 2.x release line records that progression:

| Release | What it represented |
| --- | --- |
| `V2.3.0` | A major round of bug fixes and more reliable quote handling |
| `V2.4.0` | Command history and home-directory display |
| `V2.5.0` | More complete token parsing for quotes and escapes |
| `V2.6.5` | Code cleanup and additional fixes |
| `V2.7.8` | Continued refinement toward a more usable shell |

These releases were steps in learning how the pieces fit together, not an attempt to recreate every feature of Bash.

**Version 3 marks a change in direction:** from adding shell features one by one toward building a clearer, more maintainable foundation that can grow alongside the OJ ecosystem.

## What OJC-shell is today

OJC-shell is intentionally small. It aims to provide a useful command-line environment while keeping its implementation understandable and open to improvement.

Current project capabilities include:

- External program execution using `fork()` and `execvp()`.
- Built-in commands such as `cd`, `exit`, `help`, `about`, `his`, and `ojc`.
- Quoted arguments, escaped characters, and environment-variable expansion.
- Semicolon-separated commands.
- Multi-stage pipelines.
- Input and output redirection with `<`, `>`, and `>>`.
- Persistent command history.
- Persistent configuration and selectable terminal themes.
- A configurable prompt with the previous command's exit status.

OJC-shell uses C and Unix/POSIX interfaces to make the relationship between a command and the operating system visible in the code.

## The OJ direction

OJC-shell is not meant to exist as an isolated terminal project. It is part of a longer direction: **OJclicks OS**, an exploration of a minimal, understandable, and configurable operating-system environment.

The long-term ideas behind that direction are:

- **Minimal by design:** keep the base system focused instead of accumulating layers without a reason.
- **Understandable foundations:** prefer clear components and explicit behavior over hidden complexity.
- **User control:** make the environment configurable without making its internals inaccessible.
- **A connected OJ ecosystem:** let OJC-shell become a natural command-line interface for future OJ tools and system components.
- **Room to experiment:** use the project to explore process management, command interpretation, system interfaces, and eventually deeper operating-system integration.

These are project directions, not claims that OJclicks OS or every planned integration already exists. OJC-shell is one piece of that larger work.

## Themes and configuration

OJC-shell includes four appearance themes:

| Theme | Description |
| --- | --- |
| `oj-green` | The OJ green terminal identity |
| `monochrome` | A restrained grayscale appearance |
| `classic` | Traditional terminal colors |
| `custom` | Individually configured colors |

Examples:

```sh
ojc theme list
ojc theme set oj-green
ojc theme set custom

ojc config show
ojc config set prompt '[{status}] OJC> '
ojc config set prompt_color bright-green
ojc config set error_color bright-red
ojc config reset
```

Custom color settings include `prompt_color`, `text_color`, `error_color`, `success_color`, and `info_color`. The prompt supports the `{status}` placeholder.

Configuration is stored at:

```text
${XDG_CONFIG_HOME:-$HOME/.config}/ojc/config
```

Command history is stored at:

```text
${XDG_STATE_HOME:-$HOME/.local/state}/ojc/history
```

## Build from source

### Requirements

- GCC or Clang with C11 support.
- GNU Readline development headers and library.
- GNU Make.

### Build and run

From the `src/` directory:

```sh
make
./ojcsh
```

Run the available regression tests:

```sh
make test
```

Remove generated build files:

```sh
make clean
```

## Scope and limitations

OJC-shell is a learning-oriented Unix-like shell, not a drop-in replacement for Bash, Zsh, or POSIX `sh`.

Features such as job control, command substitution, globbing, shell functions, and full POSIX shell compatibility are outside its current implementation. The project will grow deliberately, with correctness and a maintainable design taking priority over feature count.

## Contributing

Issues, bug reports, and thoughtful contributions are welcome. When reporting a problem, include the command that triggered it, the expected behavior, the actual behavior, and relevant build or runtime output.

## License

OJC-shell is free software distributed under the **GNU General Public License v3.0 or later**. See [LICENSE](LICENSE).

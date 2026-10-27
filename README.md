# OJC-shell

**A small shell, and the beginning of a much longer journey.**

OJC-shell is a lightweight, Unix-like command-line shell written in C. It began as a hands-on experiment in understanding how a shell works beneath the prompt: reading commands, parsing tokens, creating processes, connecting pipes, and communicating with the operating system.

OJC-shell was an early part of the OJ journey. Its development has now reached its final release, **V3.0.0**. The next chapter is a separate project: **OJ-shell (OJS)**, being developed for the future **OJKeys OS**.

> Build the foundation. Understand every layer. Keep control.

---

## Project status: final release

> OJC-shell has reached **V3.0.0**, its final planned release. Active development has ended, and no further feature releases are planned for this repository.

This does not mean the project is being erased or forgotten. The source code, release history, and documentation will remain here as a record of the work and learning that went into it. OJC-shell can still be explored, built, and used as-is, but it should be considered a completed project rather than an actively developed shell.

The next chapter is **OJ-shell (OJS)**, a separate project intended for **OJKeys OS**. It carries forward the experience gained here, but it is not simply another OJC-shell release.

## A note from the creator

OJC-shell means more to me than a collection of C files and terminal commands. It was one of my first serious C projects, and one of the projects closest to my heart. I started it to learn by building something real, and along the way it became a place where I could see my own progress: every bug I chased, every feature I added, every release, and every small improvement.

It is difficult to close a chapter you have spent so much time with. Part of me would happily keep adding things to OJC-shell forever. But projects, like the people who build them, need room to move forward. I would rather give this project a proper ending than keep extending it without a clear purpose.

Thank you to everyone who tried OJC-shell, followed its releases, reported issues, or simply took an interest in it. And to this little shell itself: thank you for being part of the beginning. **V3.0.0 is the final release, but it is not the end of the journey that started here.**

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

**V3.0.0 is the closing point of this project's development.** It represents the work completed in OJC-shell and marks the transition to a new, separate project rather than the start of another development phase here.

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

OJC-shell was created during an earlier stage of the OJ ecosystem, alongside the **OJclicks OS** vision. The direction has since evolved. The next command-line project is **OJ-shell (OJS)**, intended for **OJKeys OS**. OJS is a separate project; this repository remains the home of OJC-shell and its history.

The long-term ideas behind that direction are:

- **Minimal by design:** keep the base system focused instead of accumulating layers without a reason.
- **Understandable foundations:** prefer clear components and explicit behavior over hidden complexity.
- **User control:** make the environment configurable without making its internals inaccessible.
- **A connected OJ ecosystem:** build tools that fit coherently into the wider OJ environment.
- **Room to experiment:** continue exploring process management, command interpretation, system interfaces, and deeper operating-system integration in future projects.

These are long-term directions, not claims that OJKeys OS or every planned integration already exists. OJC-shell remains a completed piece of the earlier journey.

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

OJC-shell is a completed, learning-oriented Unix-like shell, not a drop-in replacement for Bash, Zsh, or POSIX `sh`.

Features such as job control, command substitution, globbing, shell functions, and full POSIX shell compatibility are outside its final implementation. These limitations are part of the project's documented scope; future development belongs in separate OJ projects.

## Contributing

This repository is kept available for learning, reference, and historical context. Since OJC-shell has reached its final planned release, active development and feature work are not expected.

## License

OJC-shell is free software distributed under the **GNU General Public License v3.0 or later**. See [LICENSE](LICENSE).

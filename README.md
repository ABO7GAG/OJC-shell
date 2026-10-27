# OJC-shell

A lightweight Unix-like shell written in C, built as a learning project for understanding command parsing, processes, pipes, and operating-system interfaces. Part of the OJclicks OS vision.

## Features

- External commands via `fork()` and `execvp()`.
- Built-ins: `cd`, `exit`, `help`, `about`, `his`, and `ojc`.
- Quoted arguments, escapes, and `$NAME`, `${NAME}`, `$?` expansion.
- Semicolon-separated commands, multi-stage pipelines, and `<`, `>`, `>>` redirection.
- Persistent history and appearance configuration.
- Selectable terminal themes and custom prompt colors.

OJC-shell implements a deliberately small shell grammar. It is not a replacement for Bash, Zsh, or POSIX `sh`.

## Build

Requires a C11 compiler, GNU Readline development files, and Make.

Run from `src/`:

```sh
make
./ojcsh
make test
make clean
```

## Themes

Available themes: `oj-green`, `monochrome`, `classic`, and `custom`.

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

Custom color keys: `prompt_color`, `text_color`, `error_color`, `success_color`, and `info_color`. Supported values include `default`, the standard terminal colors, `gray`, and `bright-*` variants. The prompt supports `{status}` for the previous command's exit status.

Configuration: `${XDG_CONFIG_HOME:-$HOME/.config}/ojc/config`.

History: `${XDG_STATE_HOME:-$HOME/.local/state}/ojc/history`.

## Limitations

Job control, command substitution, globbing, shell functions, and full POSIX compatibility are not implemented. This remains a learning and experimentation project, not a production shell.

## License

GNU General Public License v3.0. See [LICENSE](LICENSE).

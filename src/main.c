#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "shell.h"
#include "parser.h"
#include "executor.h"
#include "theme.h"
#include "config.h"

int last_status = 0;
char histfile[1024] = "";
char elapsed_str[32] = "";
OJCConfig shell_config;

static void save_history(void) {
    if (histfile[0]) (void)write_history(histfile);
}

static void setup_history_path(void) {
    const char *state = getenv("XDG_STATE_HOME");
    const char *home = getenv("HOME");
    char dir[1024], parent[1024];

    if (state && *state) {
        if (snprintf(dir,sizeof(dir),"%s/ojc",state) >= (int)sizeof(dir)) return;
        if (mkdir(state,0700) && access(state,F_OK)) return;
    } else if (home && *home) {
        if (snprintf(parent,sizeof(parent),"%s/.local",home) >= (int)sizeof(parent)) return;
        (void)mkdir(parent,0700);
        if (snprintf(parent,sizeof(parent),"%s/.local/state",home) >= (int)sizeof(parent)) return;
        (void)mkdir(parent,0700);
        if (snprintf(dir,sizeof(dir),"%s/ojc",parent) >= (int)sizeof(dir)) return;
    } else return;

    if (mkdir(dir,0700) && access(dir,F_OK)) return;
    (void)snprintf(histfile,sizeof(histfile),"%s/history",dir);
}

int main(void) {
    char *input;

    (void)config_load(&shell_config);
    setup_history_path();
    if (histfile[0]) (void)read_history(histfile);
    atexit(save_history);
    rl_catch_signals = 1;
    rl_readline_name = "ojc";

    for (;;) {
        char cwd[1024], display[1024], prompt[256], timestr[64] = "";
        struct winsize ws = {0};
        struct tm *tm_info;
        time_t now;
        int cols = 80, used, spaces;
        const char *home = getenv("HOME");

        if (!getcwd(cwd,sizeof(cwd))) snprintf(cwd,sizeof(cwd),"?");
        if (home && *home && !strncmp(cwd,home,strlen(home)) &&
            (cwd[strlen(home)] == '/' || cwd[strlen(home)] == '\0'))
            snprintf(display,sizeof(display),"~%s",cwd+strlen(home));
        else snprintf(display,sizeof(display),"%s",cwd);

        now = time(NULL);
        tm_info = localtime(&now);
        if (tm_info) (void)strftime(timestr,sizeof(timestr),"%a %b %d %H:%M:%S",tm_info);
        if (ioctl(STDOUT_FILENO,TIOCGWINSZ,&ws) == 0 && ws.ws_col > 0) cols = ws.ws_col;

        printf("%s%s\033[0m",theme_color(&shell_config,"info"),display);
        used = (int)strlen(display) + (int)strlen(elapsed_str) + (int)strlen(timestr);
        spaces = (cols-used)/2;
        if (spaces < 1) spaces = 1;
        while (spaces--) putchar(' ');
        printf("%s%s\033[0m",theme_color(&shell_config,"success"),elapsed_str);
        used = (int)strlen(display)+(int)strlen(elapsed_str);
        spaces = cols-used-(int)strlen(timestr);
        if (spaces < 1) spaces = 1;
        while (spaces--) putchar(' ');
        printf("%s\n",timestr);

        theme_build_prompt(&shell_config,last_status,prompt,sizeof(prompt));
        input = readline(prompt);
        if (!input) {
            putchar('\n');
            break;
        }
        if (*input) add_history(input);

        {
            char *parts[64];
            int count = split_on_delim(input,parts,64);
            int i;
            if (count < 0) last_status = 2;
            else for (i=0;i<count;++i) {
                char *cmd = parts[i];
                while (*cmd == ' ' || *cmd == '\t') ++cmd;
                if (*cmd) {
                    fputs(theme_color(&shell_config,"text"),stdout);
                    fflush(stdout);
                    run_command(cmd);
                    fputs("\033[0m",stdout);
                }
            }
        }
        free(input);
    }
    save_history();
    return last_status;
}

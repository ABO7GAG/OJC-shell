#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <readline/history.h>
#include "shell.h"
#include "builtins.h"
#include "config.h"

static void show_config(void) {
    printf("theme=%s\nprompt=%s\nprompt_color=%s\ntext_color=%s\nerror_color=%s\nsuccess_color=%s\ninfo_color=%s\n",
        shell_config.theme,shell_config.prompt_format,shell_config.prompt_color,
        shell_config.text_color,shell_config.error_color,shell_config.success_color,
        shell_config.info_color);
}
static int save_config(void) {
    if (config_save(&shell_config)) {
        fprintf(stderr,"ojc: unable to save configuration\n");
        last_status=1;
    } else last_status=0;
    return last_status;
}
static int ojc_command(char **a) {
    if (!a[1] || !strcmp(a[1],"help")) {
        puts("ojc theme list | set <oj-green|monochrome|classic|custom>");
        puts("ojc config show | set <key> <value> | reset");
        puts("Custom keys: prompt, prompt_color, text_color, error_color, success_color, info_color");
        last_status=0; return 1;
    }
    if (!strcmp(a[1],"theme")) {
        if (a[2] && !strcmp(a[2],"list") && !a[3]) {
            puts("oj-green\nmonochrome\nclassic\ncustom"); last_status=0;
        } else if (a[2] && !strcmp(a[2],"set") && a[3] && !a[4]) {
            if (config_set_option(&shell_config,"theme",a[3])) {
                fprintf(stderr,"ojc: unknown theme: %s\n",a[3]); last_status=2;
            } else (void)save_config();
        } else { fprintf(stderr,"usage: ojc theme list | set <name>\n"); last_status=2; }
        return 1;
    }
    if (!strcmp(a[1],"config")) {
        if (a[2] && !strcmp(a[2],"show") && !a[3]) {
            show_config(); last_status=0;
        } else if (a[2] && !strcmp(a[2],"reset") && !a[3]) {
            if (config_reset(&shell_config)) { perror("ojc: config reset"); last_status=1; }
            else { puts("OJC configuration reset."); last_status=0; }
        } else if (a[2] && !strcmp(a[2],"set") && a[3] && a[4] && !a[5]) {
            if (config_set_option(&shell_config,a[3],a[4])) {
                fprintf(stderr,"ojc: invalid setting or value: %s\n",a[3]); last_status=2;
            } else (void)save_config();
        } else { fprintf(stderr,"usage: ojc config show | set <key> <value> | reset\n"); last_status=2; }
        return 1;
    }
    fprintf(stderr,"ojc: unknown command: %s\n",a[1]); last_status=2;
    return 1;
}

int run_builtin(char **args) {
    if (!args || !args[0]) return 0;
    if (!strcmp(args[0],"exit")) {
        puts("as you like...");
        exit(last_status);
    }
    if (!strcmp(args[0],"cd")) {
        const char *target=args[1]?args[1]:getenv("HOME");
        if (!target || chdir(target)) {
            fprintf(stderr,"ojc: cd: %s: %s\n",target?target:"(HOME is not set)",
                    target?strerror(errno):"HOME is not set");
            last_status=1;
        } else last_status=0;
        return 1;
    }
    if (!strcmp(args[0],"his")) {
        HIST_ENTRY **history=history_list();
        if (history) for (int i=0;history[i];++i) printf("%d %s\n",i+1,history[i]->line);
        last_status=0; return 1;
    }
    if (!strcmp(args[0],"who?")) {
        puts("        >_\n OJC // SHELL\n\n [ built from silence ]\n [ executed in control ]\n\n      — ojclicks —");
        last_status=0; return 1;
    }
    if (!strcmp(args[0],"hello")) {
        puts("hello from OJC-shell, a vision for OJclicks OS...");
        last_status=0; return 1;
    }
    if (!strcmp(args[0],"help")) {
        puts("Builtins: cd, exit, his, who?, hello, about, ojc, ojcshfetch, help");
        last_status=0; return 1;
    }
    if (!strcmp(args[0],"about")) {
        puts("OJC-shell — a minimal Unix-like shell written in C");
        puts("Part of the OJclicks OS journey");
        puts("Not here to compete with Bash... just here to understand it.");
        last_status=0; return 1;
    }
    if (!strcmp(args[0],"ojc")) return ojc_command(args);
    if (!strcmp(args[0],"ojcshfetch")) {
        puts("\n>_ O J C   S H E L L _<\n");
        last_status=0; return 1;
    }
    return 0;
}

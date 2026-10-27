#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include "config.h"

static int config_path(char *out, size_t size, int create) {
    const char *base = getenv("XDG_CONFIG_HOME");
    char home_base[OJC_CONFIG_PATH_MAX];
    if (!base || !*base) {
        const char *home = getenv("HOME");
        if (!home || !*home) return -1;
        if (snprintf(home_base, sizeof(home_base), "%s/.config", home) >= (int)sizeof(home_base)) return -1;
        base = home_base;
    }
    if (create && mkdir(base, 0700) && errno != EEXIST) return -1;
    if (snprintf(out, size, "%s/ojc", base) >= (int)size) return -1;
    if (create && mkdir(out, 0700) && errno != EEXIST) return -1;
    {
        char dir[OJC_CONFIG_PATH_MAX];
        if (snprintf(dir, sizeof(dir), "%s", out) >= (int)sizeof(dir)) return -1;
        if (snprintf(out, size, "%s/config", dir) >= (int)size) return -1;
    }
    return 0;
}

static int valid_color(const char *v) {
    static const char *const colors[] = {
        "default","black","red","green","yellow","blue","magenta","cyan",
        "white","gray","bright-red","bright-green","bright-yellow",
        "bright-blue","bright-magenta","bright-cyan","bright-white"
    };
    size_t i;
    for (i = 0; i < sizeof(colors)/sizeof(colors[0]); ++i)
        if (!strcmp(v, colors[i])) return 1;
    return 0;
}

static int copy_value(char *dst, size_t cap, const char *src) {
    size_t i, n = strlen(src);
    if (n >= cap) return -1;
    for (i = 0; i < n; ++i)
        if ((unsigned char)src[i] < 32 || (unsigned char)src[i] == 127) return -1;
    memcpy(dst, src, n + 1);
    return 0;
}

void config_defaults(OJCConfig *c) {
    if (!c) return;
    memset(c, 0, sizeof(*c));
    strcpy(c->theme, "oj-green");
    strcpy(c->prompt_format, "[{status}]OJC> ");
    strcpy(c->prompt_color, "green");
    strcpy(c->text_color, "default");
    strcpy(c->error_color, "red");
    strcpy(c->success_color, "green");
    strcpy(c->info_color, "cyan");
}

int config_set_option(OJCConfig *c, const char *key, const char *value) {
    if (!c || !key || !value) return -1;
    if (!strcmp(key, "theme")) {
        if (strcmp(value,"oj-green") && strcmp(value,"monochrome") &&
            strcmp(value,"classic") && strcmp(value,"custom")) return -1;
        return copy_value(c->theme, sizeof(c->theme), value);
    }
    if (!strcmp(key, "prompt") || !strcmp(key, "prompt_format"))
        return copy_value(c->prompt_format, sizeof(c->prompt_format), value);
    if (!valid_color(value)) return -1;
    if (!strcmp(key, "prompt_color")) return copy_value(c->prompt_color,sizeof(c->prompt_color),value);
    if (!strcmp(key, "text_color")) return copy_value(c->text_color,sizeof(c->text_color),value);
    if (!strcmp(key, "error_color")) return copy_value(c->error_color,sizeof(c->error_color),value);
    if (!strcmp(key, "success_color")) return copy_value(c->success_color,sizeof(c->success_color),value);
    if (!strcmp(key, "info_color")) return copy_value(c->info_color,sizeof(c->info_color),value);
    return -1;
}

static char *trim(char *s) {
    char *e;
    while (isspace((unsigned char)*s)) ++s;
    e = s + strlen(s);
    while (e > s && isspace((unsigned char)e[-1])) *--e = '\0';
    return s;
}

int config_load(OJCConfig *c) {
    char path[OJC_CONFIG_PATH_MAX], line[256];
    FILE *f;
    if (!c) return -1;
    config_defaults(c);
    if (config_path(path, sizeof(path), 0)) return -1;
    f = fopen(path, "r");
    if (!f) return errno == ENOENT ? 0 : -1;
    while (fgets(line, sizeof(line), f)) {
        char *key, *value, *eq, *start;
        line[strcspn(line, "\r\n")] = '\0';
        start = line;
        while (isspace((unsigned char)*start)) ++start;
        if (!*start || *start == '#') continue;
        eq = strchr(start, '=');
        if (!eq) continue;
        *eq = '\0';
        key = trim(start);
        value = eq + 1;
        if (strcmp(key, "prompt") && strcmp(key, "prompt_format")) value = trim(value);
        (void)config_set_option(c, key, value);
    }
    if (ferror(f)) { fclose(f); return -1; }
    fclose(f);
    return 0;
}

int config_save(const OJCConfig *c) {
    char path[OJC_CONFIG_PATH_MAX], temp[OJC_CONFIG_PATH_MAX];
    FILE *f;
    if (!c || config_path(path, sizeof(path), 1)) return -1;
    if (snprintf(temp, sizeof(temp), "%s.tmp", path) >= (int)sizeof(temp)) return -1;
    f = fopen(temp, "w");
    if (!f) return -1;
    if (fprintf(f, "# OJC-shell configuration\ntheme=%s\nprompt=%s\nprompt_color=%s\ntext_color=%s\nerror_color=%s\nsuccess_color=%s\ninfo_color=%s\n",
        c->theme,c->prompt_format,c->prompt_color,c->text_color,c->error_color,c->success_color,c->info_color) < 0) {
        fclose(f); unlink(temp); return -1;
    }
    if (fclose(f)) { unlink(temp); return -1; }
    if (rename(temp, path)) { unlink(temp); return -1; }
    return 0;
}

int config_reset(OJCConfig *c) {
    config_defaults(c);
    return config_save(c);
}

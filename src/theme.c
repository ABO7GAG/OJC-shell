#include <stdio.h>
#include <string.h>
#include "theme.h"

static const char *ansi(const char *name) {
    if (!strcmp(name,"black")) return "\033[30m";
    if (!strcmp(name,"red")) return "\033[31m";
    if (!strcmp(name,"green")) return "\033[32m";
    if (!strcmp(name,"yellow")) return "\033[33m";
    if (!strcmp(name,"blue")) return "\033[34m";
    if (!strcmp(name,"magenta")) return "\033[35m";
    if (!strcmp(name,"cyan")) return "\033[36m";
    if (!strcmp(name,"white")) return "\033[37m";
    if (!strcmp(name,"gray")) return "\033[90m";
    if (!strcmp(name,"bright-red")) return "\033[91m";
    if (!strcmp(name,"bright-green")) return "\033[92m";
    if (!strcmp(name,"bright-yellow")) return "\033[93m";
    if (!strcmp(name,"bright-blue")) return "\033[94m";
    if (!strcmp(name,"bright-magenta")) return "\033[95m";
    if (!strcmp(name,"bright-cyan")) return "\033[96m";
    if (!strcmp(name,"bright-white")) return "\033[97m";
    return "\033[0m";
}

const char *theme_color(const OJCConfig *c, const char *role) {
    const char *name = "default";
    if (!c || !role) return "\033[0m";
    if (!strcmp(c->theme,"oj-green")) {
        if (!strcmp(role,"prompt") || !strcmp(role,"success")) name = "green";
        else if (!strcmp(role,"error")) name = "red";
        else if (!strcmp(role,"info")) name = "cyan";
    } else if (!strcmp(c->theme,"monochrome")) {
        if (!strcmp(role,"prompt")) name = "white";
        else if (!strcmp(role,"error")) name = "bright-white";
        else name = "gray";
    } else if (!strcmp(c->theme,"classic")) {
        if (!strcmp(role,"prompt")) name = "cyan";
        else if (!strcmp(role,"error")) name = "red";
        else if (!strcmp(role,"success")) name = "green";
        else if (!strcmp(role,"info")) name = "yellow";
    } else {
        if (!strcmp(role,"prompt")) name = c->prompt_color;
        else if (!strcmp(role,"text")) name = c->text_color;
        else if (!strcmp(role,"error")) name = c->error_color;
        else if (!strcmp(role,"success")) name = c->success_color;
        else if (!strcmp(role,"info")) name = c->info_color;
    }
    return ansi(name);
}

static void append(char *buf, size_t cap, size_t *used, const char *s, size_t n) {
    size_t room;
    if (!cap || *used >= cap - 1) return;
    room = cap - 1 - *used;
    if (n > room) n = room;
    memcpy(buf + *used, s, n);
    *used += n;
    buf[*used] = '\0';
}

void theme_build_prompt(const OJCConfig *c, int status, char *buf, size_t cap) {
    char status_text[16];
    const char *fmt, *role = status == 0 ? "prompt" : "error";
    size_t used = 0;
    if (!buf || !cap) return;
    buf[0] = '\0';
    snprintf(status_text, sizeof(status_text), "%d", status);
    fmt = c ? c->prompt_format : "[{status}]OJC> ";
    append(buf, cap, &used, "\001", 1);
    append(buf, cap, &used, theme_color(c, role), strlen(theme_color(c, role)));
    append(buf, cap, &used, "\002", 1);
    while (*fmt && used < cap - 1) {
        if (!strncmp(fmt, "{status}", 8)) {
            append(buf, cap, &used, status_text, strlen(status_text));
            fmt += 8;
        } else {
            append(buf, cap, &used, fmt++, 1);
        }
    }
    append(buf, cap, &used, "\001\033[0m\002", 6);
}

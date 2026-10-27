#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../config.h"
#include "../theme.h"

int main(void) {
    char dir[]="/tmp/ojc-config-test-XXXXXX";
    char prompt[256];
    OJCConfig config,loaded;
    if (!mkdtemp(dir) || setenv("XDG_CONFIG_HOME",dir,1)) return 1;
    config_defaults(&config);
    if (config_set_option(&config,"theme","custom") ||
        config_set_option(&config,"prompt_color","bright-green") ||
        config_set_option(&config,"prompt","ojc:{status}$ ")) return 2;
    if (config_save(&config) || config_load(&loaded)) return 3;
    if (strcmp(loaded.theme,"custom") ||
        strcmp(loaded.prompt_color,"bright-green") ||
        strcmp(loaded.prompt_format,"ojc:{status}$ ")) return 4;
    theme_build_prompt(&loaded,12,prompt,sizeof(prompt));
    if (!strstr(prompt,"ojc:12$ ") || !strchr(prompt,'\001') ||
        !strchr(prompt,'\002')) return 5;
    puts("OJC config/theme tests passed.");
    return 0;
}

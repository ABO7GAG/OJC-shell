#ifndef OJC_THEME_H
#define OJC_THEME_H
#include <stddef.h>
#include "config.h"
const char *theme_color(const OJCConfig *config, const char *role);
void theme_build_prompt(const OJCConfig *config, int status, char *buffer, size_t size);
#endif

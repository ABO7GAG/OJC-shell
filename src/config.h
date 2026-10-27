#ifndef OJC_CONFIG_H
#define OJC_CONFIG_H
#define OJC_CONFIG_PATH_MAX 1024
typedef struct { char theme[32]; char prompt_format[128]; char prompt_color[16]; char text_color[16]; char error_color[16]; char success_color[16]; char info_color[16]; } OJCConfig;
void config_defaults(OJCConfig *config);
int config_load(OJCConfig *config);
int config_save(const OJCConfig *config);
int config_reset(OJCConfig *config);
int config_set_option(OJCConfig *config, const char *key, const char *value);
#endif

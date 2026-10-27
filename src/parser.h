#ifndef PARSER_H
#define PARSER_H
int parse_input(const char *input, char **args, int max_args);
int split_on_delim(char *input, char **parts, int max_parts);
#endif

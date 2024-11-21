#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

void trim_whitespace(char *str);
void parse_irc_message(const char *message, char *prefix, char *params, size_t params_size);

#endif
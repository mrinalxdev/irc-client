#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Function to trim leading and trailing whitespace from a string
void trim_whitespace(char *str);

// Function to parse an IRC message into components
void parse_irc_message(const char *message, char *prefix, char *command, char *params, size_t params_size);

#endif // UTILS_H

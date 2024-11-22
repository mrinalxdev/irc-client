#include <string.h>
#include <ctype.h>
#include "utils.h"

void trim_whitespace(char *str) {
    char *end;

    // Trim leading spaces
    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return; // All spaces?

    // Trim trailing spaces
    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    // Null-terminate after the last non-space character
    *(end + 1) = '\0';
}

// Parses an IRC message into components
void parse_irc_message(const char *message, char *prefix, char *command, char *params, size_t params_size) {
    const char *start = message;

    // Extract prefix if it starts with ':'
    if (*start == ':') {
        start++;
        while (*start && *start != ' ') {
            *prefix++ = *start++;
        }
        *prefix = '\0';
        if (*start) start++;
    } else {
        *prefix = '\0';
    }

    // Extract command
    while (*start && *start != ' ') {
        *command++ = *start++;
    }
    *command = '\0';
    if (*start) start++;

    // Extract parameters (truncate if needed)
    strncpy(params, start, params_size - 1);
    params[params_size - 1] = '\0';
}
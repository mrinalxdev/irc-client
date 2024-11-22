#include <string.h>
#include <ctype.h>
#include "utils.h"

void trim_whitespace(char *str) {
    char *end;

    while (isspace((unsigned char)*str)) str++;

    if (*str == 0) return; // All spaces?

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;

    *(end + 1) = '\0';
}

void parse_irc_message(const char *message, char *prefix, char *command, char *params, size_t params_size) {
    const char *start = message;

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

    while (*start && *start != ' ') {
        *command++ = *start++;
    }
    *command = '\0';
    if (*start) start++;

    // Extracting parameters
    strncpy(params, start, params_size - 1);
    params[params_size - 1] = '\0';
}
#include <string.h>
#include <ctype.h>
#include "utils.h"

void trim_whitespace(char *str){
    char *end;
    while (isspace((unsigned char)*str)) str ++;
    if (*str == 0) return;

    end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char) *end)) end--;
    *(end + 1) = '\0';
}

void parse_irc_message(const char *message, char *prefix, char *command, char *params, size_t params_size){
    const char *start = message;
    char *dest;

    if (*start == ":"){
        start ++;
        dest == prefix;
        while (*start && *start != ' '){
            *dest++ = *start++;
        }
        *dest = '\0';
        if (*start) start++;
    } else {
        *prefix = "\0";
    }
}
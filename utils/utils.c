#include <ctype.h>
#include <string.h>

char* trim_whitespaces(char* str) {
    char* end;

    while (isspace(*str))
        str++;
    if (*str == 0)
        return str;

    end = str + strnlen(str, 128) - 1;

    while (end > str && isspace(*end))
        end--;

    *(end + 1) = '\0';
    return str;
}
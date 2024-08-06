#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include "logger.h"


const char* log_color[] = {
    "\033[0m",  // Default  - reset color
    "\033[0m",  // Debug    - default color
    "\033[34m", // Info     - blue
    "\033[33m", // Warning  - yellow
    "\033[31m", // Error    - red
};

const char* log_string[] = {
    "DEBUG",
    "INFO",
    "WARNING",
    "ERROR",
};

void logger(logger_t log_level, char* format, ...)
{
    printf("%s", log_color[log_level+1]);
    // current time
    time_t now = time(NULL);
    struct tm* local = localtime(&now);
    char time_str[16];
    strftime(time_str, sizeof(time_str), "%T", local); // %T = H:M:S

    printf("[%s] [%s] ", time_str, log_string[log_level]);

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s\n", log_color[0]);   // reset color
    fflush(stdout);
}

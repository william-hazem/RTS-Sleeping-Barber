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

void logger(logger_t log_level, char* format, ...)
{
    printf("%s", log_color[log_level+1]);
    time_t now;
    time(&now);
    printf("[DATA] -> ", ctime(&now));

    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("%s", log_color[0]);   // reset color
    fflush(stdout);
}
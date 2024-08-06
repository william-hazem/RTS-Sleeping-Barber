#ifndef LOGGER_H
#define LOGGR_H

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logger_t;

void logger(logger_t log_level, char* format, ...);

#endif // !LOGGER_H
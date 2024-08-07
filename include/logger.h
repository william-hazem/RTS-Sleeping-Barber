#ifndef LOGGER_H
#define LOGGR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    LOG_DEBUG = 0,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
} logger_t;

void logger(logger_t log_level, char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // !LOGGER_H
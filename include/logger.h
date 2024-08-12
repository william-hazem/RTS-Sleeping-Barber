/**
 * @file logger.h
 * @brief Minimal (color output) thread-safe logger 
 * @version 0.1
 * @date 2024-08-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

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

/**
 * @brief Initialize thread resources
 */
void logger_init();

/**
 * @brief Send formatted message to console output
 * 
 * @param log_level log level
 * @param format message
 * @param ... extra arguments
 */
void logger(logger_t log_level, char* format, ...);

#ifdef __cplusplus
}
#endif

#endif // !LOGGER_H
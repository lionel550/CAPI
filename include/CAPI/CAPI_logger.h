#ifndef CAPI_LOGGER_H_
#define CAPI_LOGGER_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    CAPI_INFO,
    CAPI_WARN,
    CAPI_ERROR,
} CAPI_LogLevel;

void CAPI_SetLogLevel(CAPI_LogLevel log_level);

void CAPI_EnableLog(bool enabled);

void CAPI_WriteLog(FILE *stream, CAPI_LogLevel log_level, char *fmt, ...);

#define CAPI_LOG_INFO(fmt, ...) CAPI_WriteLog(stderr, CAPI_INFO, fmt, ##__VA_ARGS__)
#define CAPI_LOG_WARN(fmt, ...) CAPI_WriteLog(stderr, CAPI_WARN, fmt, ##__VA_ARGS__)
#define CAPI_LOG_ERROR(fmt, ...) CAPI_WriteLog(stderr, CAPI_ERROR, fmt, ##__VA_ARGS__)

#endif // CAPI_LOGGER_H_

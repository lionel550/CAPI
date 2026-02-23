#ifndef CAPI_LOGGER_H_
#define CAPI_LOGGER_H_

#include <stdio.h>
#include <stdbool.h>

typedef enum {
    CAPI_ERR,
    CAPI_WARN,
    CAPI_INFO,
} CAPI_LogLevel;

void CAPI_SetLogLevel(CAPI_LogLevel log_level);

void CAPI_DisableLog(bool disabled);

void CAPI_WriteLog(FILE *stream, CAPI_LogLevel log_level, char *fmt, ...);

#define CAPI_LOG_INFO(message, ...) CAPI_WriteLog(stdout, CAPI_INFO, message, ##__VA_ARGS__);
#define CAPI_LOG_WARN(message, ...) CAPI_WriteLog(stdout, CAPI_WARN, message, ##__VA_ARGS__);
#define CAPI_LOG_ERROR(message, ...) CAPI_WriteLog(stdout, CAPI_ERR, message, ##__VA_ARGS__);

#endif // CAPI_LOGGER_H_

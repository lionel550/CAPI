#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include "CAPI/CAPI_logger.h"

static CAPI_LogLevel CAPI_GlobalLogLevel = CAPI_INFO;

static bool CAPI_IsValidLogLevel(CAPI_LogLevel log_level)
{
    switch (log_level) {
        case CAPI_INFO:
        case CAPI_WARN:
        case CAPI_ERR:
            return true;
        default:
            return false;
    }
}

static char *CAPI_LogLevelToString(CAPI_LogLevel log_level)
{
    switch (log_level) {
        case CAPI_INFO:
            return "INFO";
        case CAPI_WARN:
            return "WARN";
        case CAPI_ERR:
            return "ERROR";
        default:
            return NULL;
    }
}

void CAPI_SetLogLevel(CAPI_LogLevel log_level)
{
    if (!CAPI_IsValidLogLevel(log_level))
        return;

    CAPI_GlobalLogLevel = log_level;
}

void CAPI_WriteLog(FILE *stream, CAPI_LogLevel log_level, char *fmt, ...)
{
    if (!CAPI_IsValidLogLevel(log_level) || log_level > CAPI_GlobalLogLevel)
        return;

    va_list ap;
    va_start(ap, fmt);
    fprintf(stream, "%s: ", CAPI_LogLevelToString(log_level));
    fprintf(stream, fmt, ap);
    fprintf(stream, "\n");
    va_end(ap);

    fflush(stream);
}

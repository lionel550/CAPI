#include <stdio.h>
#include <stdarg.h>
#include "CAPI/CAPI_error.h"

static __thread CAPI_Error last_error;

char *CAPI_GetLastErrorMessage()
{
    return last_error.error_message;
}

CAPI_ErrorCode CAPI_GetLastErrorCode()
{
    return last_error.error_code;
}

void CAPI_SetErrorCode(CAPI_ErrorCode error_code, char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(last_error.error_message, sizeof(last_error.error_message), fmt, ap);
    va_end(ap);

    last_error.error_code = error_code;
}


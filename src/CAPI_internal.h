#ifndef CAPI_INTERNAL_H_
#define CAPI_INTERNAL_H_

#include "CAPI/CAPI_error.h"

CAPI_ErrorCode CAPI_RegisterSignalActions();

int CAPI_ShouldKeepRunning();

void CAPI_FreeEndpointRegister();

void CAPI_SetErrorCode(CAPI_ErrorCode error_code, char *fmt, ...);

#endif // CAPI_INTERNAL_H_

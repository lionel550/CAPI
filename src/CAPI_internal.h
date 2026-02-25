#ifndef CAPI_INTERNAL_H_
#define CAPI_INTERNAL_H_

#include "CAPI/CAPI_error.h"
#include "CAPI/CAPI_logger.h"
#include "CAPI/CAPI_types.h"

CAPI_ErrorCode CAPI_RegisterSignalActions();

int CAPI_ShouldKeepRunning();

void CAPI_FreeEndpointRegister();

void CAPI_SetErrorCode(CAPI_ErrorCode error_code, char *fmt, ...);

CAPI_ErrorCode CAPI_ReadClientRequest(int client_sockfd, char **buffer, size_t *total_bytes_read);

#endif // CAPI_INTERNAL_H_

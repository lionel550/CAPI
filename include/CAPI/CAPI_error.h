#ifndef CAPI_ERROR_H_
#define CAPI_ERROR_H_

typedef enum {
    CAPI_SUCCESS,
    CAPI_ERR_MALLOC,
    CAPI_ERR_FORK,
    CAPI_ERR_SERVER,
    CAPI_ERR_INVALID_PARAMETER,
    CAPI_ERROR,
} CAPI_ErrorCode;

typedef struct {
    char error_message[256];
    CAPI_ErrorCode error_code;
} CAPI_Error;

char *CAPI_GetLastErrorMessage();

CAPI_ErrorCode CAPI_GetLastErrorCode();

#endif // CAPI_ERROR_H_

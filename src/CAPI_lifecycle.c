#include <stdlib.h>
#include <sys/wait.h>
#include "CAPI_internal.h"
#include "CAPI/CAPI_logger.h"

void CAPI_ReapedChildProcesses()
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

CAPI_ErrorCode CAPI_RegisterExitCalls()
{
    if (atexit(CAPI_FreeEndpointRegister) != 0 || atexit(CAPI_ReapedChildProcesses) != 0)
    {
        CAPI_SetErrorCode(CAPI_ERR_INIT, "Unable to register exit calls.");
        return CAPI_ERR_INIT;
    }

    return CAPI_SUCCESS;
}

void __attribute__((constructor(101))) CAPI_Init()
{
    if ((CAPI_RegisterSignalActions() | CAPI_RegisterExitCalls()) != CAPI_SUCCESS)
    {
        CAPI_LOG_ERROR(CAPI_GetLastErrorMessage());
        exit(CAPI_GetLastErrorCode());
    }
}


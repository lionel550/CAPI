#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "CAPI_internal.h"

void CAPI_ReapedChildProcesses()
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

CAPI_ErrorCode CAPI_RegisterExitCalls()
{
    if ((atexit(CAPI_FreeEndpointRegister) | atexit(CAPI_ReapedChildProcesses)) != 0)
    {
        CAPI_SetErrorCode(CAPI_ERROR, "Unable to register exit calls.");
        return CAPI_ERROR;
    }

    return CAPI_SUCCESS;
}

void __attribute__((constructor(101))) CAPI_Init()
{
    if ((CAPI_RegisterSignalActions() | CAPI_RegisterExitCalls()) != CAPI_SUCCESS)
    {
        fprintf(stderr, CAPI_GetLastErrorMessage());
        exit(CAPI_GetLastErrorCode());
    }
}


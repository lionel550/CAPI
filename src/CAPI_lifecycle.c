#include <stdlib.h>
#include <sys/wait.h>
#include "CAPI_internal.h"

void CAPI_ReapedChildProcess()
{
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

void __attribute__((constructor(101))) CAPI_Init()
{
    if ((CAPI_RegisterSignalActions() | atexit(CAPI_FreeEndpointRegister) | atexit(CAPI_ReapedChildProcess)) == -1)
    {
        exit(EXIT_FAILURE);
    }
}


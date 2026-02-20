#include "CAPI_internal.h"
#include <stdlib.h>

void __attribute__((constructor(101))) CAPI_Init()
{
    if ((CAPI_RegisterSignalActions() | atexit(CAPI_FreeEndpointRegister)) == -1)
    {
        exit(EXIT_FAILURE);
    }
}


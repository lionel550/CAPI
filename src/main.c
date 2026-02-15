#include <stdio.h>
#include "CAPI/CAPI.h"

int main()
{
    if (CAPI_ServerInit() == -1)
    {
        fprintf(stderr, "Server init error\n");
        return -1;
    }

    if (CAPI_RunServer() == -1)
    {
        fprintf(stderr, "Run server error\n");
        return -1;
    }

    return 0;
}

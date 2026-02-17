#ifndef CAPI_REGISTRY_ROUTE_
#define CAPI_REGISTRY_ROUTE_

#include <stdlib.h>
#include "CAPI/CAPI_types.h"

int CAPI_RegisterEndpoint(CAPI_HttpMethod http_method, char *route, CAPI_ApiCall api_call);

CAPI_ApiCall CAPI_GetApiCallFor(CAPI_HttpMethod http_method, char *route);

void CAPI_FreeRegister();

#define CONCAT_HIDDEN(a, b, c, d) a ## _ ## b ## _ ## c ## _ ## d
#define CONCAT(a, b, c, d) CONCAT_HIDDEN(a, b, c, d)

#define REGISTER_ENDPOINT(http_method, route, api_call) \
    void __attribute__((constructor)) CONCAT(register_endpoint, http_method, api_call, __COUNTER__)() { \
        if (CAPI_RegisterEndpoint(http_method, route, api_call) < 0) { \
            CAPI_FreeRegister(); \
            exit(EXIT_FAILURE); \
        } \
    }

#endif // CAPI_REGISTRY_ROUTE_

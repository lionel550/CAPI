#ifndef CAPI_ENDPOINT_REGISTRY_H_
#define CAPI_ENDPOINT_REGISTRY_H_

#include <stdlib.h>
#include "CAPI/CAPI_types.h"

int CAPI_RegisterEndpoint(CAPI_HttpMethod http_method, char *route, CAPI_ApiCall api_call);

CAPI_ApiCall CAPI_GetApiCallFor(CAPI_HttpMethod http_method, char *route);

void CAPI_FreeEndpointRegister();

#define CAPI_CONCAT_HIDDEN(a, b) a ## _ ## b
#define CAPI_CONCAT(a, b) CAPI_CONCAT_HIDDEN(a, b)

#define CAPI_REGISTER_ENDPOINT(http_method, route, api_call) \
    void __attribute__((constructor(102))) CAPI_CONCAT(register_endpoint, __COUNTER__)() { \
        if (CAPI_RegisterEndpoint(http_method, route, api_call) == -1) { \
            exit(EXIT_FAILURE); \
        } \
    }

#endif // CAPI_ENDPOINT_REGISTRY_H_

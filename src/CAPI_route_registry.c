#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "CAPI/CAPI_route_registry.h"

typedef struct {
    CAPI_HTTP_METHOD method;
    char *route;
    CAPI_API_CALL func_call;
} CAPI_Endpoint;

// TODO: Replace this structure by a tree to speed up endpoint research
typedef struct {
    CAPI_Endpoint *endpoints;
    int capacity;
    int tail;
} CAPI_EndpointRegister;

CAPI_EndpointRegister endpoint_register = {.endpoints = NULL, .capacity = 0, .tail = -1};

static bool CAPI_AddEndpoint(CAPI_Endpoint endpoint)
{
    if (endpoint_register.endpoints == NULL || endpoint_register.capacity == endpoint_register.tail + 1)
    {
        CAPI_Endpoint *tmp_ptr = (CAPI_Endpoint *) malloc(sizeof(CAPI_Endpoint) * (endpoint_register.capacity + 8));
        if (tmp_ptr == NULL) return false;

        for (int i = 0; i < endpoint_register.capacity; i++)
        {
            tmp_ptr[i] = endpoint_register.endpoints[i];
        }

        free(endpoint_register.endpoints);
        endpoint_register.endpoints = tmp_ptr;
        endpoint_register.capacity += 8;
    }

    endpoint_register.tail += 1;
    endpoint_register.endpoints[endpoint_register.tail] = endpoint; // TODO: The string is not copy

    return true;
}

static void CAPI_DeinitRegister()
{
    free(endpoint_register.endpoints);
    endpoint_register.endpoints = NULL;
    endpoint_register.capacity = 0;
    endpoint_register.tail = -1;
}

void CAPI_RegisterEndpoint(CAPI_HTTP_METHOD method, char *route, CAPI_API_CALL call)
{
    CAPI_Endpoint endpoint = {method, route, call};

    if (!CAPI_AddEndpoint(endpoint))
    {
        CAPI_DeinitRegister();
        exit(EXIT_FAILURE);
    }
}

CAPI_API_CALL CAPI_GetApiCallFor(CAPI_HTTP_METHOD method, char *route) {
    for (int i = 0; i <= endpoint_register.tail; i++) {
        if (endpoint_register.endpoints[i].method == method && strcmp(endpoint_register.endpoints[i].route, route) == 0)
        {
            return endpoint_register.endpoints[i].func_call;
        }
    }

    return NULL;
}

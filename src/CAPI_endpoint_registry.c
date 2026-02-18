#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "CAPI/CAPI_types.h"

// TODO: Replace this structure by a tree to speed up endpoint research

typedef struct {
    CAPI_Endpoint *endpoints;
    int capacity;
    int size;
} CAPI_EndpointRegister;

static CAPI_EndpointRegister endpoint_register = {.endpoints = NULL, .capacity = 0, .size = 0};

CAPI_ApiCall CAPI_GetApiCallFor(CAPI_HttpMethod http_method, char *route) {
    for (int i = 0; i < endpoint_register.size; i++) {
        if (endpoint_register.endpoints[i].http_method == http_method 
                && strcmp(endpoint_register.endpoints[i].route, route) == 0)
        {
            return endpoint_register.endpoints[i].api_call;
        }
    }

    return NULL;
}

static int CAPI_AddEndpoint(CAPI_Endpoint endpoint)
{
    if (CAPI_GetApiCallFor(endpoint.http_method, endpoint.route) != NULL)
        return -1;

    if (endpoint_register.endpoints == NULL || endpoint_register.capacity == endpoint_register.size)
    {
        CAPI_Endpoint *tmp_ptr = (CAPI_Endpoint *) malloc(
                sizeof(CAPI_Endpoint) * (endpoint_register.capacity + 8));

        if (tmp_ptr == NULL)
            return -1;

        for (int i = 0; i < endpoint_register.size; i++)
            tmp_ptr[i] = endpoint_register.endpoints[i];

        free(endpoint_register.endpoints);

        endpoint_register.endpoints = tmp_ptr;
        endpoint_register.capacity += 8;
    }

    char *tmp_ptr = (char *) malloc(strlen(endpoint.route) + 1);
    
    if (tmp_ptr == NULL)
        return -1;
    
    strcpy(tmp_ptr, endpoint.route);
    
    endpoint_register.endpoints[endpoint_register.size].route = tmp_ptr;
    endpoint_register.endpoints[endpoint_register.size].http_method = endpoint.http_method; 
    endpoint_register.endpoints[endpoint_register.size].api_call = endpoint.api_call; 
    endpoint_register.size += 1;

    return 0;
}

void CAPI_FreeEndpointRegister()
{
    for (int i = 0; i < endpoint_register.size; i++)
        free(endpoint_register.endpoints[i].route);

    free(endpoint_register.endpoints);
    
    endpoint_register.endpoints = NULL;
    endpoint_register.capacity = 0;
    endpoint_register.size = 0;
}

int CAPI_RegisterEndpoint(CAPI_HttpMethod http_method, char *route, CAPI_ApiCall api_call)
{
    CAPI_Endpoint endpoint = {http_method, route, api_call};

    if (CAPI_AddEndpoint(endpoint) < 0)
        return -1;

    return 0;
}


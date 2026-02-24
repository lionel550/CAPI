#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "CAPI_internal.h"

// TODO: Replace this structure by a tree to speed up endpoint research

typedef struct {
    CAPI_Endpoint *endpoints;
    int capacity;
    int size;
} CAPI_EndpointRegister;

static CAPI_EndpointRegister endpoint_register = {.endpoints = NULL, .capacity = 0, .size = 0};

CAPI_ApiCall CAPI_GetApiCallFor(CAPI_HttpMethod http_method, char *route)
{
    if (!CAPI_IsValidHttpMethod(http_method) || route == NULL)
        return NULL;

    for (int i = 0; i < endpoint_register.size; i++) {
        if (endpoint_register.endpoints[i].http_method == http_method 
                && strcmp(endpoint_register.endpoints[i].route, route) == 0)
        {
            return endpoint_register.endpoints[i].api_call;
        }
    }

    return NULL;
}

static CAPI_ErrorCode CAPI_AddEndpoint(CAPI_Endpoint endpoint)
{
    if (CAPI_GetApiCallFor(endpoint.http_method, endpoint.route) != NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_INVALID_PARAMETER,
                "The endpoint `%s %s` already exist.",
                CAPI_HttpMethodToString(endpoint.http_method), endpoint.route);

        return CAPI_ERR_INVALID_PARAMETER;
    }

    if (endpoint_register.endpoints == NULL || endpoint_register.capacity == endpoint_register.size)
    {
        CAPI_Endpoint *tmp_ptr = (CAPI_Endpoint *) malloc(sizeof(CAPI_Endpoint) * (endpoint_register.capacity + 8));

        if (tmp_ptr == NULL)
        {
            CAPI_SetErrorCode(CAPI_ERR_MALLOC,
                    "Unable to allocate memory to register the endpoint `%s %s`.",
                    CAPI_HttpMethodToString(endpoint.http_method), endpoint.route);

            return CAPI_ERR_MALLOC;
        }

        for (int i = 0; i < endpoint_register.size; i++)
            tmp_ptr[i] = endpoint_register.endpoints[i];

        free(endpoint_register.endpoints);

        endpoint_register.endpoints = tmp_ptr;
        endpoint_register.capacity += 8;
    }

    char *tmp_ptr = (char *) malloc(strlen(endpoint.route) + 1);
    
    if (tmp_ptr == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_MALLOC, "Unable allocate memory for the route `%s`.", endpoint.route);
        return CAPI_ERR_MALLOC;
    }
    
    strcpy(tmp_ptr, endpoint.route);
    
    endpoint_register.endpoints[endpoint_register.size].route = tmp_ptr;
    endpoint_register.endpoints[endpoint_register.size].http_method = endpoint.http_method; 
    endpoint_register.endpoints[endpoint_register.size].api_call = endpoint.api_call; 
    endpoint_register.size += 1;

    return CAPI_SUCCESS;
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

CAPI_ErrorCode CAPI_RegisterEndpoint(CAPI_HttpMethod http_method, char *route, CAPI_ApiCall api_call)
{
    if (!CAPI_IsValidHttpMethod(http_method) || route == NULL || api_call == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_INVALID_PARAMETER, "Cannot register endpoint: invalid parameter.");
        return CAPI_ERR_INVALID_PARAMETER;
    }

    CAPI_Endpoint endpoint = {http_method, route, api_call};

    return CAPI_AddEndpoint(endpoint); 
}


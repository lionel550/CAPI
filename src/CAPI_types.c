#include <stdbool.h>
#include <stddef.h>
#include "CAPI/CAPI_types.h"

bool CAPI_IsValidHttpMethod(CAPI_HttpMethod http_method)
{
    switch (http_method)
    {
        case GET:
        case POST:
        case PUT:
        case PATCH:
        case DELETE:
            return true;
        default:
            return false;
    }
}

char *CAPI_HttpMethodToString(CAPI_HttpMethod http_method)
{
    switch (http_method)
    {
        case GET: return "GET";
        case POST: return "POST";
        case PUT: return "PUT";
        case PATCH: return "PATCH";
        case DELETE: return "DELETE";
        default: return NULL;
    }
}

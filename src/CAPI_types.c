#include <stdbool.h>
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

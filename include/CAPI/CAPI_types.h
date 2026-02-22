#ifndef CAPI_TYPES_H_
#define CAPI_TYPES_H_

#include <stdbool.h>
// TODO: Complete the definition of CAPI_HttpMethod, CAPI_HttpRequest and CAPI_HttpResponse

typedef enum {
    GET,
    POST,
    PUT,
    PATCH,
    DELETE,
} CAPI_HttpMethod;

bool CAPI_IsValidHttpMethod(CAPI_HttpMethod http_method);
char *CAPI_HttpMethodToString(CAPI_HttpMethod http_method);

typedef struct {
    CAPI_HttpMethod method;
} CAPI_HttpRequest;

typedef struct {
    char *reponse;
} CAPI_HttpResponse;

typedef void (*CAPI_ApiCall)(CAPI_HttpRequest*, CAPI_HttpResponse*);

typedef struct {
    CAPI_HttpMethod http_method;
    char *route;
    CAPI_ApiCall api_call;
} CAPI_Endpoint;

#endif // CAPI_TYPES_H_

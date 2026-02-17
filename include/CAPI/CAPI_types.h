#ifndef CAPI_TYPES_
#define CAPI_TYPES_

// TODO: Complete the definition of CAPI_HttpMethod, CAPI_HttpRequest and CAPI_HttpResponse

typedef enum {
    GET,
    POST,
    PUT,
    PATCH,
    DELETE,
} CAPI_HttpMethod;

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

#endif // CAPI_TYPES_

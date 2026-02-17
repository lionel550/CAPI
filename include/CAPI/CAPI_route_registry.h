#ifndef CAPI_REGISTRY_ROUTE
#define CAPI_REGISTRY_ROUTE

typedef enum {
    GET,
    POST,
    PUT,
    // TODO: Support more HTTP methods
} CAPI_HTTP_METHOD;

typedef struct {
    CAPI_HTTP_METHOD method;
} CAPI_Request;

typedef struct {
    char *reponse;
} CAPI_Response;

typedef void (*CAPI_API_CALL)(CAPI_Request*, CAPI_Response*);

void CAPI_RegisterEndpoint(CAPI_HTTP_METHOD method, char *route, CAPI_API_CALL call);

CAPI_API_CALL CAPI_GetApiCallFor(CAPI_HTTP_METHOD method, char *route);

#define CONCAT_HIDDEN(a, b) a ## b
#define CONCAT(a, b) CONCAT_HIDDEN(a, b)

#define REGISTER_ENDPOINT(method, route, api_call) \
    void __attribute__((constructor)) CONCAT(register_endpoint_, __LINE__)() { \
        CAPI_RegisterEndpoint(method, route, api_call); \
    }

#endif // CAPI_REGISTRY_ROUTE

#include <stdio.h>
#include "CAPI/CAPI.h"

void hello(CAPI_HttpRequest *request, CAPI_HttpResponse *response) {
    (void) request;
    (void) response;
    printf("Hello from hello\n");
}
CAPI_REGISTER_ENDPOINT(GET, "/hello", hello)

int main()
{
    CAPI_ApiCall api_call = CAPI_GetApiCallFor(GET, "/hello");
    
    if (api_call == NULL) {
        printf("Error\n");
        return -1;
    }

    api_call(NULL, NULL);
    CAPI_RunServer();
    return 0;
}

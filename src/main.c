#include <stdio.h>
#include "CAPI/CAPI.h"

void hello(CAPI_Request *request, CAPI_Response *response) {
    printf("Hello from hello\n");
}

REGISTER_ENDPOINT(GET, "/hello", hello)

int main()
{
    CAPI_API_CALL api_call = CAPI_GetApiCallFor(GET, "/hello");
    if (api_call == NULL) {
        printf("Error\n");
        return -1;
    }
    api_call(NULL, NULL);
    // CAPI_RunServer();
    return 0;
}

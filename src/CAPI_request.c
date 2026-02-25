#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "CAPI_internal.h"

#define BUFFER_SIZE 1024

// TODO: Set a time out
// TODO: Resolve the 'adress already' use problem
// TODO: Fix the log proble (CAPI_LOG_INFO("%lu", sizeof(int)))
CAPI_ErrorCode CAPI_ReadClientRequest(int client_sockfd, char **buffer, size_t *total_bytes_read)
{
    char read_buffer[BUFFER_SIZE] = {0};
    char *collect_buffer = malloc(BUFFER_SIZE);
    size_t collect_buffer_capacity = BUFFER_SIZE;
    char *cursor = collect_buffer;

    if (collect_buffer == NULL)
    {
        CAPI_SetErrorCode(CAPI_ERR_MALLOC, "Unable to allocate request buffer.");
        return CAPI_ERR_MALLOC;
    }

    memset(collect_buffer, '\0', BUFFER_SIZE);

    for (;;)
    {
        ssize_t bytes_read = recv(client_sockfd, read_buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read == -1)
        {
            CAPI_SetErrorCode(CAPI_ERR_SYS_CALL, strerror(errno));
            return CAPI_ERR_SYS_CALL;
        }
        
        if (bytes_read == 0)
        {
            CAPI_SetErrorCode(CAPI_ERR_CLIENT, "Client disconnected.");
            return CAPI_ERR_CLIENT;
        }
   
        read_buffer[bytes_read] = '\0';

        if (collect_buffer_capacity - strlen(collect_buffer) < (size_t) (bytes_read + 1))
        {
            if (collect_buffer_capacity == 4 * BUFFER_SIZE)
            {
                CAPI_SetErrorCode(CAPI_ERR_CLIENT, "The client exceeded the maximum request size.");
                return CAPI_ERR_CLIENT;
            }

            char * tmp = realloc(collect_buffer, collect_buffer_capacity + BUFFER_SIZE);
            
            if (tmp == NULL)
            {
                CAPI_SetErrorCode(CAPI_ERR_MALLOC, "Unable to allocate request buffer.");
                free(collect_buffer);
                return CAPI_ERR_MALLOC;
            }
            
            collect_buffer = tmp;
            collect_buffer_capacity += BUFFER_SIZE;
        }

        strcpy(cursor, read_buffer);
        cursor += bytes_read;

        if (strstr(collect_buffer, "\r\n\r\n") != NULL)
        {
            *buffer = collect_buffer;
            *total_bytes_read = strlen(collect_buffer);
            return CAPI_SUCCESS;
        }
    }

    return CAPI_UNKNOWN_ERR;
}

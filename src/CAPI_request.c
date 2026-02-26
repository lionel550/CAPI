#include <errno.h>
#include <sys/time.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include "CAPI_internal.h"

#define BUFFER_SIZE 1024

// TODO: Set a time out
// TODO: Resolve the 'adress already' use problem
// TODO: Define it own header file ?
// TODO: Clean Up error code and rename CAPI_SetErrorCode to CAPI_SetError
// Create a default config structure in server for: timeout, buffsize, max request size, ....
CAPI_ErrorCode CAPI_ReadRequest(int client_sockfd, char **buffer, size_t *total_bytes_read)
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
    
    struct timeval timeout = {0};
    socklen_t optlen = sizeof(timeout);

    if (getsockopt(client_sockfd, SOL_SOCKET, SO_RCVTIMEO, (void *)&timeout, &optlen) == -1)
    {
        CAPI_SetErrorCode(CAPI_ERR_SYS_CALL, "Unable to get the receive timeout: %s", strerror(errno));
        free(collect_buffer);
        return CAPI_ERR_SYS_CALL;
    }

    struct timeval start, now;
    if (gettimeofday(&start, NULL) == -1)
    {
        CAPI_SetErrorCode(CAPI_UNKNOWN_ERR, "Unable to get time of day: %s", strerror(errno));
        free(collect_buffer);
        return CAPI_UNKNOWN_ERR;
    }
    
    for (;;)
    {
        if (timeout.tv_usec > 0 || timeout.tv_sec > 0)
        {
            if (gettimeofday(&now, NULL) == -1)
            {
                CAPI_SetErrorCode(CAPI_UNKNOWN_ERR, "Unable to get time of day: %s", strerror(errno));
                free(collect_buffer);
                return CAPI_UNKNOWN_ERR;
            }

            long elapsed = (now.tv_sec - start.tv_sec) * 1000000 + (now.tv_usec - start.tv_usec);

            if (elapsed >= (timeout.tv_sec * 1000000 + timeout.tv_usec))
            {
                CAPI_SetErrorCode(CAPI_UNKNOWN_ERR, "Timeout: the client take too long to send request data.");
                free(collect_buffer);
                return CAPI_UNKNOWN_ERR;
            }
        }

        ssize_t bytes_read = recv(client_sockfd, read_buffer, BUFFER_SIZE - 1, 0);
        
        if (bytes_read == -1)
        {
            CAPI_SetErrorCode(CAPI_ERR_SYS_CALL, "Error while receiving request data");
            free(collect_buffer);
            return CAPI_ERR_SYS_CALL;
        }
        
        if (bytes_read == 0)
        {
            CAPI_SetErrorCode(CAPI_ERR_CLIENT, "Client disconnected.");
            free(collect_buffer);
            return CAPI_ERR_CLIENT;
        }
   
        read_buffer[bytes_read] = '\0';

        if (collect_buffer_capacity - strlen(collect_buffer) < (size_t) (bytes_read + 1))
        {
            if (collect_buffer_capacity == 4 * BUFFER_SIZE)
            {
                CAPI_SetErrorCode(CAPI_ERR_CLIENT, "The client exceeded the maximum request size.");
                free(collect_buffer);
                return CAPI_ERR_CLIENT;
            }

            char *tmp = realloc(collect_buffer, collect_buffer_capacity + BUFFER_SIZE);
            
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

    CAPI_SetErrorCode(CAPI_UNKNOWN_ERR, "Unexpected error happen where receiving client data.");
    free(collect_buffer);
    return CAPI_UNKNOWN_ERR;
}

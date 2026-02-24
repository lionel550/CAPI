#include <errno.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include "CAPI_internal.h"
#include "CAPI/CAPI_logger.h"

// TODO: Implement dynamic array for request queue
#define DEFAULT_BINDING_ADRESS  INADDR_LOOPBACK
#define DEAULT_BINDING_PORT     8080
#define REQUEST_QUEUE_SIZE      15

static int CAPI_CreateSocket()
{
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd == -1)
        goto socket_return_error;

    int flags = fcntl(server_sockfd, F_GETFL, 0);
    
    if (flags == -1)
        goto socket_clean_and_return_error;

    if (fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK) == -1)
        goto socket_clean_and_return_error;

    return server_sockfd;

socket_clean_and_return_error:
    close(server_sockfd);

socket_return_error:
    CAPI_SetErrorCode(CAPI_ERR_SERVER, "Unable to create socket: %s", strerror(errno));
    return -1;
}

static CAPI_ErrorCode CAPI_BindSocket(int server_sockfd, in_addr_t adress, int port)
{
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(adress);

    if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        CAPI_SetErrorCode(CAPI_ERR_SERVER, "Unable to bind socket: %s", strerror(errno));
        return CAPI_ERR_SERVER;
    }
    
    return CAPI_SUCCESS;
}

// TODO: Execute valgrind with all option and resolve fd errors
static CAPI_ErrorCode CAPI_AcceptLoop(int server_sockfd)
{
    if (listen(server_sockfd, REQUEST_QUEUE_SIZE) == -1)
        goto accept_loop_return_error;

    struct pollfd pfd = {
        .fd = server_sockfd,
        .events = POLLIN,
        .revents = 0
    };

    CAPI_LOG_INFO("running on http://127.0.0.1:8080/ (Press CTRL+C to quit)");

    while (CAPI_ShouldKeepRunning())
    {
        int ready = poll(&pfd, 1, -1);

        if (ready == -1)
        {
            if (errno == EINTR) continue;
            goto accept_loop_return_error;
        }

        if (pfd.revents & POLLIN) {
            int request_sockfd = accept(server_sockfd, NULL, NULL);
            
            if (request_sockfd == -1)
            {
                CAPI_LOG_ERROR(strerror(errno));
                continue;
            }
            
            pid_t pid = fork();
            
            if (pid == -1)
            {
                CAPI_LOG_ERROR(strerror(errno));
                close(request_sockfd);
            }
            else if (pid == 0)
            {
                CAPI_LOG_INFO("request received !");

                close(server_sockfd);
                
                const char *http_response = 
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/plain\r\n"
                "Content-Length: 44\r\n"
                "Connection: close\r\n"
                "\r\n"
                "Unexpected item in bagging area. Be courage.";

                if (send(request_sockfd, http_response, strlen(http_response), 0) == -1)
                {
                    perror("send");
                    close(request_sockfd);
                    return CAPI_ERR_SERVER;
                }

                close(request_sockfd);
                return CAPI_SUCCESS;
            }
            else
            {
                close(request_sockfd);
            }    
        }
    }

    return CAPI_SUCCESS;

accept_loop_return_error:
    CAPI_SetErrorCode(CAPI_ERR_SERVER, "Unable accept incomming connection: %s", strerror(errno));
    return CAPI_ERR_SERVER;
}

void CAPI_RunServer()
{
    int server_sockfd = CAPI_CreateSocket();
    
    if (server_sockfd == -1)
        goto run_server_exit_error;

    if (CAPI_BindSocket(server_sockfd, DEFAULT_BINDING_ADRESS, DEAULT_BINDING_PORT) != CAPI_SUCCESS 
            || CAPI_AcceptLoop(server_sockfd) != CAPI_SUCCESS)
        goto run_server_clean_and_exit_error;

    close(server_sockfd);
    exit(EXIT_SUCCESS);

run_server_clean_and_exit_error:
    close(server_sockfd);

run_server_exit_error:
    CAPI_LOG_ERROR(CAPI_GetLastErrorMessage());
    exit(CAPI_GetLastErrorCode());
}

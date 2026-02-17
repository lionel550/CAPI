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
#include <sys/wait.h>
#include <signal.h>

// TODO: Implement dynamic array for request queue
// TODO: <Ctrl + c> to stop the server
#define DEFAULT_BINDING_ADRESS  INADDR_LOOPBACK
#define DEAULT_BINDING_PORT     8080
#define REQUEST_QUEUE_SIZE      15

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

static int CAPI_CreateSocket()
{
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    int flags = fcntl(server_sockfd, F_GETFL, 0);
    
    if (flags == -1)
    {
        perror("fcntl(F_GETFL)");
        close(server_sockfd);
        return -1;
    }

    if (fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        close(server_sockfd);
        return -1;
    }

    return server_sockfd;
}

static int CAPI_BindSocket(int server_sockfd, in_addr_t adress, int port)
{
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = htonl(adress);

    if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        return -1;
    }
    
    return 0;
}

static int CAPI_AcceptLoop(int server_sockfd)
{
    if (listen(server_sockfd, REQUEST_QUEUE_SIZE) == -1)
    {
        perror("listen");
        return -1;
    }

    struct pollfd pfd = {
        .fd = server_sockfd,
        .events = POLLIN,
        .revents = 0
    };

    for (;;)
    {
        int ready = poll(&pfd, 1, -1);

        if (ready == -1)
        {
            if (errno == EINTR) continue;
            perror("poll");
            return -1;
        }

        if (pfd.revents & POLLIN) {
            int request_sockfd = accept(server_sockfd, NULL, NULL);
            
            if (request_sockfd == -1)
            {
                perror("accept");
            }
            
            pid_t pid = fork();
            
            if (pid == -1)
            {
                perror("fork");
                close(request_sockfd);
            }
            else if (pid == 0)
            {
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
                    return -1;
                }

                close(request_sockfd);
                return 0;
            }
            else
            {
                close(request_sockfd);
            }    
        }
    }

    return -1;
}

void CAPI_RunServer()
{
    int server_sockfd = CAPI_CreateSocket();
    
    if (server_sockfd == -1)
    {
        exit(EXIT_FAILURE);
    }

    if (CAPI_BindSocket(server_sockfd, DEFAULT_BINDING_ADRESS, DEAULT_BINDING_PORT) == -1)
    {
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }

    struct sigaction sa = {.sa_handler = sigchld_handler, .sa_flags = SA_RESTART};
    sigaction(SIGCHLD, &sa, NULL);

    if (CAPI_AcceptLoop(server_sockfd) == -1)
    {
        close(server_sockfd);
        exit(EXIT_FAILURE);
    }
}

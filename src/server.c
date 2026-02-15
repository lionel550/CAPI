#include <errno.h>
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

// TODO: Create the socket at server_run and exit on error
#define DEFAULT_BINDING_ADRESS  INADDR_LOOPBACK
#define DEAULT_BINDING_PORT     8080
#define REQUEST_QUEUE_SIZE      15

static int server_sockfd = -1;

void sigchld_handler(int s) {
    while(waitpid(-1, NULL, WNOHANG) > 0);
}

static void CAPI_ServerDeinit()
{
    close(server_sockfd);
    server_sockfd = -1;
}

int CAPI_ServerInit()
{
    // Create a socket
    server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_sockfd == -1)
    {
        perror("socket");
        return -1;
    }
    
    // Set server_sockfd to nonblock
    int flags = fcntl(server_sockfd, F_GETFL, 0);
    
    if (flags == -1)
    {
        perror("fcntl(F_GETFL)");
        CAPI_ServerDeinit();
        return -1;
    }

    if (fcntl(server_sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl(F_SETFL)");
        CAPI_ServerDeinit();
        return -1;
    }

    // Bind a port and an adress to the socket
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(DEAULT_BINDING_PORT);
    server_addr.sin_addr.s_addr = htonl(DEFAULT_BINDING_ADRESS);

    if (bind(server_sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        CAPI_ServerDeinit();
        return -1;
    }
    
    return 0;
}

// TODO: Add shutdown for server_sockfd and childfd
// TODO: When signal call CAPI_ServerDeinit
int CAPI_RunServer()
{
    if (server_sockfd == -1) {
        fputs("Server is not initialize", stderr);
        return -1;
    }

    struct sigaction sa = {.sa_handler = sigchld_handler, .sa_flags = SA_RESTART};
    sigaction(SIGCHLD, &sa, NULL);

    // Listen for entry connection
    if (listen(server_sockfd, REQUEST_QUEUE_SIZE) == -1)
    {
        perror("listen");
        CAPI_ServerDeinit();
        return -1;
    }
    
    // shutdown write
    if (shutdown(server_sockfd, SHUT_WR) == -1)
    {
        perror("shutdown init");
        CAPI_ServerDeinit();
        return -1;
    }


    struct pollfd pfds[1] = {0};
    pfds[0].fd = server_sockfd;
    pfds[0].events = POLLIN;

    for (;;)
    {
        int ready = poll(pfds, sizeof(pfds) / sizeof(struct pollfd), -1);

        if (ready == -1)
        {
            if (errno == EINTR) continue;
            perror("poll");
            CAPI_ServerDeinit();
            return -1;
        }

        if (pfds[0].revents & POLLIN) {
            // Accept connection
            int request_sockfd = accept(server_sockfd, NULL, NULL);
            
            // TODO: Handle the empty list case ?
            if (request_sockfd == -1)
            {
                perror("accept");
            }

            if (shutdown(request_sockfd, SHUT_RD) == -1)
            {
                perror("shutdown request");
            }
            
            pid_t pid = fork();
            
            if (pid == -1)
            {
                // TODO: May be send code 500 ?
                perror("fork");
                close(request_sockfd);
            }
            else if (pid == 0)
            {
                close(server_sockfd);

                // Send a message
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
             
                if (shutdown(request_sockfd, SHUT_WR) == -1)
                {
                    perror("shutdown request");
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
}


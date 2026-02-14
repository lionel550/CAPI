#include <stdio.h>
#include <errno.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>

#define REQUEST_QUEUE_SIZE  15

int main()
{
    // Create a socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1)
    {
        perror("socket");
        return -1;
    }

    // Bind a port and an adress to the socket
    struct sockaddr_in server_addr = {0};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(8080);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        close(sockfd);
        return -1;
    }

    // Listen for entry connection
    if (listen(sockfd, REQUEST_QUEUE_SIZE) == -1)
    {
        perror("listen");
        close(sockfd);
        return -1;
    }

    // Accept connection
    int requestSockfd = accept(sockfd, NULL, NULL);
        
    if (requestSockfd == -1)
    {
        perror("accept");
        close(sockfd);
        return -1;
    }
     
    // Send a message
    const char *http_response = 
    "HTTP/1.1 200 OK\r\n"
    "Content-Type: text/plain\r\n"
    "Content-Length: 44\r\n"
    "Connection: close\r\n"
    "\r\n"
    "Unexpected item in bagging area. Be courage.";

    if (send(requestSockfd, http_response, strlen(http_response), 0) == -1)
    {
        perror("send");
        close(requestSockfd);
        close(sockfd);
        return -1;
    }

    close(requestSockfd);
    close(sockfd);

    fputs("Hello from main !\n", stdout);
    return 0;
}

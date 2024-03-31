#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT_TCP 33333
#define PORT_HTTP 80
#define MAX_CLIENTS 2

// Handle errors
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void handle_tcp_request(int inferno_accept);
void handle_http_request(int http_listener_accept);

int main() {
    // Socket setup for TCP
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Socket creation failed");

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_TCP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    int reuse = 1;
    if (setsockopt(inferno_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        error("Error reusing TCP address");

    if (bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Binding socket failed");

    if (listen(inferno_socket, MAX_CLIENTS) == -1)
        error("Listening failed");

    // Socket setup for HTTP
    int http_listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_listener_socket == -1)
        error("HTTP socket creation failed");

    struct sockaddr_in http_listener_address;
    memset(&http_listener_address, 0, sizeof(http_listener_address));
    http_listener_address.sin_family = AF_INET;
    http_listener_address.sin_port = htons(PORT_HTTP);
    http_listener_address.sin_addr.s_addr = inet_addr(IP);

    if (setsockopt(http_listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        error("Error reusing HTTP address");

    if (bind(http_listener_socket, (struct sockaddr *) &http_listener_address, sizeof(http_listener_address)) == -1)
        error("Binding HTTP failed");

    if (listen(http_listener_socket, MAX_CLIENTS) == -1)
        error("Listening failed");

    // Main loop
    fd_set readfds;
    int max_sd;

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(inferno_socket, &readfds);
        FD_SET(http_listener_socket, &readfds);
        max_sd = (inferno_socket > http_listener_socket) ? inferno_socket : http_listener_socket;

        if (select(max_sd + 1, &readfds, NULL, NULL, NULL) == -1)
            error("Error in select");

        // TCP request handling
        if (FD_ISSET(inferno_socket, &readfds)) {
            int inferno_accept = accept(inferno_socket, NULL, NULL);
            if (inferno_accept == -1)
                error("Accepting TCP failed");

            handle_tcp_request(inferno_accept);
        }

        // HTTP request handling
        if (FD_ISSET(http_listener_socket, &readfds)) {
            int http_listener_accept = accept(http_listener_socket, NULL, NULL);
            if (http_listener_accept == -1)
                error("Accepting HTTP failed");

            handle_http_request(http_listener_accept);
        }
    }

    close(inferno_socket);
    close(http_listener_socket);

    return 0;
}

void handle_tcp_request(int inferno_accept) {
    char request[1024];
    if (recv(inferno_accept, request, sizeof(request), 0) == -1)
        error("Error receiving TCP request");

    printf("TCP Request: %s\n", request);

    char response[1024] = "TCP Response";
    if (send(inferno_accept, response, sizeof(response), 0) == -1)
        error("Error sending TCP response");

    close(inferno_accept);
}

void handle_http_request(int http_listener_accept) {
    char request[1024];
    if (recv(http_listener_accept, request, sizeof(request), 0) == -1)
        error("Error receiving HTTP request");

    printf("HTTP Request: %s\n", request);

    char response[1024] = "HTTP Response";
    if (send(http_listener_accept, response, sizeof(response), 0) == -1)
        error("Error sending HTTP response");

    close(http_listener_accept);
}

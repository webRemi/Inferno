#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#define IP "127.0.0.1"
#define PORT_TCP 33333
#define PORT_HTTP 80
#define MAX_CLIENTS 10

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void tcp_transfer(int tcp_accept, int http_socket, int *http_listener_on);

void http_transfer(int http_accept);

void http_listener(int http_socket);

int main() {
    int http_listener_on = 0;
    puts("Initializing socket...");
    int tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcp_socket == -1)
        error("Socket creation failed");

    //preparing address tcp
    struct sockaddr_in tcp_address;
    memset(&tcp_address, 0, sizeof(tcp_address));
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(PORT_TCP);
    tcp_address.sin_addr.s_addr = inet_addr(IP);

    int reuse_tcp = 1;
    //reuse address
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_tcp, sizeof(reuse_tcp)) == -1)
        error("Error reusing tcp address");

    //reuse port
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEPORT, &reuse_tcp, sizeof(reuse_tcp)) == -1)
        perror("Error reusing tcp port");

    //bind tcp
    puts("Binding socket to Inferno address...");
    if (bind(tcp_socket, (struct sockaddr *) &tcp_address, sizeof(tcp_address)) == -1)
        error("Binding socket failed");

    //listen tcp
    printf("Listening to %s:%d\n", IP, PORT_TCP);
    if (listen(tcp_socket, 0) == -1)
        error("Listening failed");

    //http socket
    int http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1)
        error("HTTP socket creation failed");

    fd_set inferno_sockets;
    FD_ZERO(&inferno_sockets);
    FD_SET(tcp_socket, &inferno_sockets);
    int max_socket = tcp_socket;

    int client_sockets[MAX_CLIENTS] = {0};

    if (tcp_socket > max_socket) max_socket = tcp_socket;

    while (1) {
        int tcp_accept;
        fd_set save;
        save = inferno_sockets;
        //int tcp_accept;
        select(max_socket + 1, &save, NULL, NULL, NULL);
        printf("Accepting...");

        if (FD_ISSET(tcp_socket, &save)) {
            tcp_accept = accept(tcp_socket, NULL, NULL);
            if (tcp_accept == -1) {
                error("Error accepting tcp");
            }
            puts("Accepted");
            tcp_transfer(tcp_accept, http_socket, &http_listener_on);
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = tcp_accept;
                break;
            }
        }

        FD_SET(tcp_accept, &inferno_sockets);
        if (tcp_accept > max_socket) max_socket = tcp_accept;

        if (http_listener_on) {
            FD_SET(http_socket, &inferno_sockets);
            if (http_socket > max_socket) max_socket = http_socket;
        }

        if (FD_ISSET(http_socket, &save)) {
            int http_accept = accept(http_socket, NULL, NULL);

            http_transfer(http_accept);
        }
        close(tcp_accept);
    }
    close(tcp_socket);
    close(http_socket);
}

void tcp_transfer(int tcp_accept, int http_socket, int *http_listener_on) {
    char tcp_request[1024];
    printf("Receiving from client: %s\n", tcp_request);
    if (recv(tcp_accept, tcp_request, sizeof(tcp_request), 0) == -1)
        error("Error receiving from client");

    if (strcmp(tcp_request, "http") == 0 && *http_listener_on == 0) {
        *http_listener_on = 1;
        http_listener(http_socket);
    }

    char tcp_response[1024] = "HELLO FROM TCP";
    printf("Sending to client: %s\n", tcp_response);
    if (send(tcp_accept, tcp_response, sizeof(tcp_response), 0) == -1)
        error("Error sending to client");
}

void http_transfer(int http_accept) {
    char http_response[1024] = "HELLO FROM HTTP";
    if (send(http_accept, http_response, sizeof(http_response), 0) == -1)
        error("Error sending to agent");

    char http_request[1024];
    if (recv(http_accept, http_request, sizeof(http_request), 0) == -1)
        error("Error receiving from agent");
}

void http_listener(int http_socket) {
    struct sockaddr_in http_address;
    memset(&http_address, 0, sizeof(http_address));
    http_address.sin_family = AF_INET;
    http_address.sin_port = htons(PORT_HTTP);
    http_address.sin_addr.s_addr = inet_addr(IP);
    puts("HTTP listener initialized");

    //reuse http
    int reuse_http = 1;
    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_http, sizeof(reuse_http)) == -1)
        error("Error reusing http address");

    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEPORT, &reuse_http, sizeof(reuse_http)) == -1)
        error("Error reusing http port");

    //bind http
    puts("Binding http listener...");
    if (bind(http_socket, (struct sockaddr *) &http_address, sizeof(http_address)) == -1)
        error("Binding HTTP failed");

    //listen http
    puts("Listening...");
    if (listen(http_socket, 0) == -1)
        error("Failed listening");
    
}

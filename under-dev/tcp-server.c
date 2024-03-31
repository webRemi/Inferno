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
#define MAX_CLIENTS 10

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void tcp_transfer(int tcp_accept);

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

    fd_set inferno_sockets;
    FD_ZERO(&inferno_sockets);
    FD_SET(tcp_socket, &inferno_sockets);
    int max_socket = tcp_socket;

    int client_sockets[MAX_CLIENTS] = {0};

    if (tcp_socket > max_socket) max_socket = tcp_socket;

    while (1) {
        int tcp_accept = -1;
        fd_set save = inferno_sockets;
        select(max_socket + 1, &save, NULL, NULL, NULL);
        printf("Accepting...");

        if (FD_ISSET(tcp_socket, &save)) {
            tcp_accept = accept(tcp_socket, NULL, NULL);
            if (tcp_accept == -1) {
                error("Error accepting tcp");
            }
            puts("Accepted");
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            if (client_sockets[i] == 0) {
                client_sockets[i] = tcp_accept;
                break;
            }
        }

        FD_SET(tcp_accept, &inferno_sockets);
        if (tcp_accept > max_socket) max_socket = tcp_accept;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            int client_socket = client_sockets[i];
            if (FD_ISSET(client_socket, &save)) {
                tcp_transfer(client_socket);
                close(client_socket);
                FD_CLR(client_socket, &inferno_sockets);
                client_sockets[i] = 0;
            }
        }
        close(tcp_accept);
    }
    close(tcp_socket);
}

void tcp_transfer(int tcp_accept) {
    char tcp_request[1024];
    printf("Receiving from client: %s\n", tcp_request);
    if (recv(tcp_accept, tcp_request, sizeof(tcp_request), 0) == -1)
        error("Error receiving from client");

    char tcp_response[1024] = "HELLO FROM TCP";
    printf("Sending to client: %s\n", tcp_response);
    if (send(tcp_accept, tcp_response, sizeof(tcp_response), 0) == -1)
        error("Error sending to client");
}


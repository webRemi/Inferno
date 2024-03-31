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

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void http_listener(int inferno_socket, int inferno_accept);

int main() {
    int http_listener_on = 0;
    //socket tcp
    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Socket creation failed");

    //preparing address tcp
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_TCP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    int reuse = 1;
    //reuse address
    if (setsockopt(inferno_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) == -1)
        error("Error reusing tcp address");

    //reuse port
    if (setsockopt(inferno_socket, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(reuse)) == -1)
        perror("Error reusing tcp port");

    //bind tcp
    puts("Binding socket to Inferno address...");
    if (bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Binding socket failed");

    //listen tcp
    printf("Listening to %s:%d\n", IP, PORT_TCP);
    if (listen(inferno_socket, 0) == -1)
        error("Listening failed");

    while (1) {
        //accept tcp
        puts("Accepting...");
        int inferno_accept = accept(inferno_socket, NULL, NULL);
        if (inferno_accept == -1)
            error("Accepting failed");
        puts("Client connected");

        pid_t child_pid = fork();

        if (child_pid == 0) {
            while(1) {
                //child process
                close(inferno_socket);

                //work
                //read request
                char request[1024];
                if (recv(inferno_accept, request, sizeof(request), 0) == -1)
                    error("Error receiving request");
                printf("Request: %s\n", request);

                if (strcmp(request, "http") == 0) {
                    http_listener_on = 1;
                }

                //send response
                char response[1024] = "HTTP/1.0 200 OK\r\n\r\n";
                if (send(inferno_accept, response, sizeof(response), 0) == -1)
                    error("Error sending response");

                if (http_listener_on) {
                    http_listener(inferno_socket, inferno_accept);
                }
            }
            close(inferno_accept);
            exit(EXIT_SUCCESS);
        } else {
            //parent process
            close(inferno_accept);
        }
    }
}

void http_listener(int inferno_socket, int inferno_accept) {
    //http socket
    int http_listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_listener_socket == -1)
        error("HTTP socket creation failed");

    //socket http
    puts("Initializing http listener...");

    //preparing http address
    struct sockaddr_in http_listener_address;
    memset(&http_listener_address, 0, sizeof(http_listener_address));
    http_listener_address.sin_family = AF_INET;
    http_listener_address.sin_port = htons(PORT_HTTP);
    http_listener_address.sin_addr.s_addr = inet_addr(IP);
    puts("HTTP listener initialized");

    //reuse http
    int reuse_http = 1;
    if (setsockopt(http_listener_socket, SOL_SOCKET, SO_REUSEADDR, &reuse_http, sizeof(reuse_http)) == -1)
        error("Error reusing http address");

    if (setsockopt(http_listener_socket, SOL_SOCKET, SO_REUSEPORT, &reuse_http, sizeof(reuse_http)) == -1)
        error("Error reusing http port");

    //bind http
    puts("Binding http listener...");
    if (bind(http_listener_socket, (struct sockaddr *) &http_listener_address, sizeof(http_listener_address)) == -1)
        error("Binding HTTP failed");

    //listen http
    puts("Listening...");
    if (listen(http_listener_socket, 0) == -1)
        error("Failed listening");

    //while http_listener_on
    while (1) {
        //accept http
        puts("Accepting...");
        int http_listener_accept = accept(http_listener_socket, NULL, NULL);
        if (http_listener_accept == -1)
            error("Failed accepting");
        puts("Accepted");

        pid_t child_pid = fork();

        if (child_pid == 0) {
            while(1) {
                //child process
                close(http_listener_socket);
                close(inferno_socket);
                //work
                //read request from client
                char request_tcp[1024];
                if (recv(inferno_accept, request_tcp, sizeof(request_tcp), 0) == -1)
                    error("Error receiving request");
                printf("Request: %s\n", request_tcp);

                //send response to agent
                char response[1024] = "HELLO AGENT FROM SERVER";
                if (send(http_listener_accept, response, sizeof(response), 0) == -1)
                    error("Error sending response");

                char request[1024];
                //read request from agent
                if (recv(http_listener_accept, request, sizeof(request), 0) == -1)
                    error("Error receiving from agent");
                printf("Request: %s\n", request);

                char response_tcp[1024];
                strcpy(response_tcp, request);
                //send response to client
                if (send(inferno_accept, response_tcp, sizeof(response_tcp), 0) == -1)
                    error("Error sending to client");
                printf("Sending to client:\n%s\n", response_tcp);
            }
            close(http_listener_accept);
            close(inferno_accept);
            exit(EXIT_SUCCESS);
        } else {
            //parent process
            close(http_listener_accept);
            close(inferno_accept);
        }
    }
}

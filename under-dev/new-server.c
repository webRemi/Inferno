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
#define TCP_PORT 33333
#define HTTP_PORT 80
#define MAX_LISTENERS 10

struct http_listener {
    int id;
    char protocol[20];
    char address[20];
    int port;
};

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void craft_http_listener(char *request, struct http_listener *listener, int listener_id, int listener_num);

int start_http_listener(int tcp_socket, int tcp_accept, struct http_listener *listener);

//void http_interact(int tcp_socket, int http_socket);

int main() {
    struct http_listener listener[MAX_LISTENERS] = {0};
    int listener_id = 1;
    int listener_num = 0;
    int start_http_listener_on = 0;
    //socket tcp
    puts("Initializing socket...");
    int tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcp_socket == -1)
        error("Socket creation failed");

    //preparing address tcp
    struct sockaddr_in tcp_address;
    memset(&tcp_address, 0, sizeof(tcp_address));
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(TCP_PORT);
    tcp_address.sin_addr.s_addr = inet_addr(IP);

    int tcp_reuse = 1;
    //reuse address
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &tcp_reuse, sizeof(tcp_reuse)) == -1)
        error("Error reusing tcp address");

    //reuse port
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEPORT, &tcp_reuse, sizeof(tcp_reuse)) == -1)
        perror("Error reusing tcp port");

    //bind tcp
    puts("Binding socket to Inferno address...");
    if (bind(tcp_socket, (struct sockaddr *) &tcp_address, sizeof(tcp_address)) == -1)
        error("Binding socket failed");

    //listen tcp
    printf("Listening to %s:%d\n", IP, TCP_PORT);
    if (listen(tcp_socket, 0) == -1)
        error("Listening failed");

    while (1) {
        //accept tcp
        puts("Accepting...");
        int tcp_accept = accept(tcp_socket, NULL, NULL);
        if (tcp_accept == -1)
            error("Accepting failed");
        puts("Client connected");

        pid_t tcp_child_pid = fork();

        if (tcp_child_pid == 0) {
            while(1) {
                //child process
                close(tcp_socket);

                //work
                //read request
                char request[1024];
                if (recv(tcp_accept, request, sizeof(request), 0) == -1)
                    error("Error receiving request");
                printf("Request: %s\n", request);

                if (strncmp(request, "http ", 5) == 0) {
                    if (listener_id == MAX_LISTENERS)
                        printf("Sorry no more than %s listeners allowed", MAX_LISTENERS);
                    craft_http_listener(request, &listener[listener_num], listener_id, listener_num);
                    int http_socket = start_http_listener(tcp_socket, tcp_accept, &listener[listener_num]);
                    listener_id++;
                    listener_num++;
                    pid_t http_child_pid = fork();
                    if (http_child_pid == 0) {
                        int http_accept;
                        while(1) {
                            http_accept = accept(http_socket, NULL, NULL);
                            if (http_accept == -1)
                               error("Accepting http failed");
                            puts("Agent connected");

                            while(1) {
                                close(http_socket);
                                //process
                            }
                        }
                        close(http_accept);
                        exit(EXIT_SUCCESS);
                    } else {
                    }
                }

                //send response
                char response[1024] = "HTTP/1.0 200 OK\r\n\r\n";

                if (strcmp(request, "listeners") == 0) {
                    char listeners[1024];
                    sprintf(listeners, "ID\tPROTOCOL\tADDRESS\tPORT\n");
                    sprintf(listeners + strlen(listeners), "==\t========\t=======\t====\n");
                    for (int i = 0; i < listener_num; i++) {
                        sprintf(listeners + strlen(listeners), "%d\t%s\t\t%s\t%d\n", listener[i].id, listener[i].protocol, listener[i].address, listener[i].port);
                    }
                    strcpy(response, listeners);
                }

                if (send(tcp_accept, response, sizeof(response), 0) == -1)
                    error("Error sending response");

            }
            close(tcp_accept);
            exit(EXIT_SUCCESS);
        } else {
            //parent process
            close(tcp_accept);
        }
    }
}

void craft_http_listener(char *request, struct http_listener *listener, int id, int listener_num) {
    char *protocol = strtok(request, " ");
    char *address = strtok(NULL, " ");
    char *port_string = strtok(NULL, "\n");

    listener->id = id;
    strcpy(listener->protocol, protocol);
    strcpy(listener->address, address);
    listener->port = atoi(port_string);
}

int start_http_listener(int tcp_socket, int tcp_accept, struct http_listener *listener) {
    //http socket
    int http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1)
        error("HTTP socket creation failed");

    //socket http
    puts("Initializing http listener...");

    //preparing http address
    struct sockaddr_in http_address;
    memset(&http_address, 0, sizeof(http_address));
    http_address.sin_family = AF_INET;
    http_address.sin_port = htons(listener->port);
    http_address.sin_addr.s_addr = inet_addr(listener->address);
    puts("HTTP listener initialized");

    //reuse http
    int http_reuse = 1;
    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEADDR, &http_reuse, sizeof(http_reuse)) == -1)
        error("Error reusing http address");

    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEPORT, &http_reuse, sizeof(http_reuse)) == -1)
        error("Error reusing http port");

    //bind http
    puts("Binding http listener...");
    if (bind(http_socket, (struct sockaddr *) &http_address, sizeof(http_address)) == -1)
        error("Binding HTTP failed");

    //listen http
    puts("Listening...");
    if (listen(http_socket, 0) == -1)
        error("Failed listening");

    return http_socket;
}

/*void http_interact(int tcp_socket, int tcp_accept, int http_socket) {
    //while start_http_listener_on
    while (1) {
        //accept http
        puts("Accepting...");
        int http_accept = accept(http_socket, NULL, NULL);
        if (http_accept == -1)
            error("Failed accepting");
        puts("Accepted");

        pid_t child_pid = fork();

        if (child_pid == 0) {
            while(1) {
                //child process
                close(http_socket);
                close(tcp_socket);
                //work
                //read request from client
                char request_tcp[1024];
                if (recv(tcp_accept, request_tcp, sizeof(request_tcp), 0) == -1)
                    error("Error receiving request");
                printf("Request: %s\n", request_tcp);

                //send response to agent
                char response[1024] = "HELLO AGENT FROM SERVER";
                if (send(http_accept, response, sizeof(response), 0) == -1)
                    error("Error sending response");

                char request[1024];
                //read request from agent
                if (recv(http_accept, request, sizeof(request), 0) == -1)
                    error("Error receiving from agent");
                printf("Request: %s\n", request);

                char response_tcp[1024];
                strcpy(response_tcp, request);
                //send response to client
                if (send(tcp_accept, response_tcp, sizeof(response_tcp), 0) == -1)
                    error("Error sending to client");
                printf("Sending to client:\n%s\n", response_tcp);
            }
            close(http_accept);
            close(tcp_accept);
            exit(EXIT_SUCCESS);
        } else {
            //parent process
            close(http_accept);
            close(tcp_accept);
        }
    }
}*/


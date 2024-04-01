#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT_HTTP 8000

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

int main() {
    //socket http
    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Error initializing socket");

    //preparing http address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_HTTP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect http
    puts("Connecting...");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Error connecting");
    puts("Connected");

    while (1) {
        //receiving http payload
        char receive[1024];
        if (recv(inferno_socket, receive, sizeof(receive), 0) == -1)
            error("Error receiving payload");
        printf("Receiving:\n\n%s\n\n", receive);

        //sending payload result
        char response[1024] = "HELLO FROM AGENT";
        printf("Sending: '%s' from agent...\n", response);
        if (send(inferno_socket, response, sizeof(response), 0) == -1)
            error("Error sending payload result");
        printf("Sent:\n\n'%s' from agent...\n", response);
    }
    close(inferno_socket);
}

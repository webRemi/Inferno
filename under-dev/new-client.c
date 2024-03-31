#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define IP "127.0.0.1"
#define PORT 33333
#define PORT_HTTP 80

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void session(int inferno_socket);

int main() {
    //initialize socket
    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Initializing socket failed");

    // crafting all necessary for address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect
    puts("Connecting...");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
         error("Error connecting");
    puts("Connected");

    while (1) {
         //prompt
         char request[1024];
         printf("\n[ASX]@[INFERNO]> ");
         if (fgets(request, sizeof(request), stdin) == NULL){
             perror("fgets failed");
             exit(EXIT_FAILURE);
         }
         request[strcspn(request, "\n")] = '\0';

         //sending request
         if (send(inferno_socket, request, sizeof(request), 0) == -1)
             error("Error sending command to server");

         //receving response
         char response[1024];
         if (recv(inferno_socket, response, sizeof(response), 0) == -1)
             error("Error receiving instruction from server");
         printf("Result:\n%s\n", response);

         if (strcmp(request, "http") == 0) {
             printf("HTTP Listener started");
             //session(inferno_socket);
         }
    }
    close(inferno_socket);
}

void session(int inferno_socket) {
    while (1) {
         //prompt
         char request[1024];
         printf("\n[ASX]@[SESSION]> ");
         if (fgets(request, sizeof(request), stdin) == NULL){
             perror("fgets failed");
             exit(EXIT_FAILURE);
         }
         request[strcspn(request, "\n")] = '\0';

         //sending request
         if (send(inferno_socket, request, sizeof(request), 0) == -1)
             error("Error sending command to server");

         //receving response
         char response[1024];
         if (recv(inferno_socket, response, sizeof(response), 0) == -1)
             error("Error receiving instruction from server");
         printf("Result:\n%s\n", response);
    }
    close(inferno_socket);
}

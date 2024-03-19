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
#define PORT 8000

void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

int main() {

    const char response_ok[] = "HTTP/1.0 200 OK\r\n\r\n";
    char request[1024];
    char request_copy[1024];
    //socket
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Socket creation failed");

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //bind
    printf("Binding socket to c2 address...\n");
    int inferno_bind = bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    if (inferno_bind == -1) 
        error("Binding socket failed");

    //listen
    printf("Listening to %s:%d\n", IP, PORT);
    int inferno_listen = listen(inferno_socket, 0);
    if (inferno_listen == -1) 
        error("Listening failed");

    while (1) {
        //accept
    	printf("Accepting...\n");
    	int inferno_accept = accept(inferno_socket, NULL, NULL);
        if (inferno_accept == -1)
            error("Accepting failed");

        //read
        printf("Reading request...\n");
        ssize_t inferno_read = read(inferno_accept, request, sizeof(request));
        if (inferno_read == -1)
            error("Failed reading request");
        printf("Request is: %s\n", request);

        //parsing request
        strcpy(request_copy, request);
        char *instruction = strtok(request_copy, " ");
        char *content = strtok(NULL, " ");
        char *version = strtok(NULL, "\n");
        printf("The method used was: %s\n", instruction);
        printf("The content asked was: %s\n", content);
        printf("The version used was: %s\n", version);

        //open
        printf("Opening content...\n");
        int inferno_open = open(content, O_RDONLY);
        if (inferno_open == -1)
            error("Failed open content");
        printf("Content: %s\n", content);

        char readed[1024];
        //read
        printf("Reading content...\n");
        inferno_read = read(inferno_open, readed, sizeof(readed));
        if (inferno_read == -1)
            error("Failed reading content");
        readed[strcspn(readed, "\n")] = '\0';
        printf("Content: %s\n", readed);

        //close stream
        if (close(inferno_open) == -1)
            error("Failed closing stream");

        //write status
    	printf("Sending response...\n");
        ssize_t inferno_write = write(inferno_accept, response_ok, sizeof(response_ok));
        if (inferno_write == -1)
            error("Failed sending response");

        //write file
        printf("Sending content of: %s\n", content);
        inferno_write = write(inferno_accept, readed, sizeof(readed));
        if (inferno_write == -1)
            error("Failed sending content");

        //close
        if (close(inferno_accept) == -1)
            error("Failed closing accept");
    }
    //close
    printf("Closing socket...\n");
    if (close(inferno_socket) == -1)
        error("Failed closing socket");
}

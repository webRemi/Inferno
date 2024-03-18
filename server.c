#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 8000

int main() {

    const char response_ok[] = "HTTP/1.0 200 OK\r\n\r\n";
    char request[1024];
    char request_copy[1024];
    //socket
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //bind
    printf("Binding socket to c2 address...\n");
    int inferno_bind = bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));

    //listen
    printf("Listening to %s:%d\n", IP, PORT);
    int inferno_listen = listen(inferno_socket, 0);
    while (1) {
        //accept
    	printf("Accepting...\n");
    	int inferno_accept = accept(inferno_socket, NULL, NULL);

        //read
        printf("Reading request...\n");
        ssize_t inferno_read = read(inferno_accept, request, sizeof(request));
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
        printf("Content: %s\n", content);

        char readed[1024];
        //read
        printf("Reading content...\n");
        inferno_read = read(inferno_open, readed, sizeof(readed));
        printf("Content: %s\n", readed);

        //write
    	printf("Sending response...\n");
        ssize_t inferno_write = write(inferno_accept, response_ok, sizeof(response_ok));

        //close
        close(inferno_accept);
    }
    //close
    printf("Closing socket...\n");
    close(inferno_socket);
}

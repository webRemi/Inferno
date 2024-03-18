#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(8000);

    printf("Binding socket to c2 address...\n");
    int inferno_bind = bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));

    printf("Listening...\n");
    int inferno_listen = listen(inferno_socket, 0);

    printf("Accepting...\n");
    int inferno_accept = accept(inferno_socket, NULL, NULL);
}

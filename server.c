#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 8000

int main() {

    const char buf[] = "HTTP/1.0 200 OK\r\n\r\n";

    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    printf("Binding socket to c2 address...\n");
    int inferno_bind = bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));

    printf("Listening to %s:%d\n", IP, PORT);
    int inferno_listen = listen(inferno_socket, 0);
    while (1) {
    	printf("Accepting...\n");
    	int inferno_accept = accept(inferno_socket, NULL, NULL);
    	ssize_t inferno_write = write(inferno_accept, buf, strlen(buf));
	close(inferno_accept);
    }
    printf("Closing socket...\n");
    close(inferno_socket);
}

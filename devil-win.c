#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

#define IP "192.168.1.40"
#define PORT_HTTP 80

int main() {
    puts("Hello ASX");
    WORD wVersionRequested;
    WSADATA wsadata;

    wVersionRequested = MAKEWORD(2, 2);

    WSAStartup(wVersionRequested, &wsadata);

    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_HTTP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    puts("Connecting...");
    connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    puts("Connected");

     while (1) {
        //receiving http payload
        char receive[1024];
        recv(inferno_socket, receive, sizeof(receive), 0);
        printf("Receiving:\n\n%s\n\n", receive);

        //sending payload result
        char response[1024] = "HELLO WORLD";
        printf("Sending: '%s' from agent...\n", response);
        send(inferno_socket, response, sizeof(response), 0);
        printf("Sent:\n\n'%s' from agent...\n", response);
    }

    //closing socket
    closesocket(inferno_socket);

    WSACleanup();
}

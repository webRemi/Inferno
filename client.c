#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#define IP "127.0.0.1"
#define PORT 33333
#define PORT_HTTP 80

void box_error();

//handle erros
void error(char *message) {
    char buf[1024];
    sprintf(buf, "\033[31;5;208m[!] \033[0m");
    fprintf(stderr, "%s%s: %s\n", buf, message, strerror(errno));
    exit(EXIT_FAILURE);
}

void banner();
void info();
void help_menu();
void box_info();
void box_error();
void box_success();
void client(int inferno_socket);
void session(int inferno_socket);



int main() {
    banner();
    info();

    //initialize socket
    box_info();
    puts("Initializing socket...");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Initializing socket failed");
    box_success();
    puts("Socket initialized");

    // crafting all necessary for address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect
    box_info();
    puts("Connecting to server...");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
         error("Error connecting to server");
    box_success();
    puts("Connected to server");
    client(inferno_socket);
}

void client(int inferno_socket) {
    while (1) {
         //prompt
         char request[1024];
         printf("\n[ASX]@[INFERNO]> ");
         if (fgets(request, sizeof(request), stdin) == NULL){
             perror("fgets failed");
             exit(EXIT_FAILURE);
         }
         request[strcspn(request, "\n")] = '\0';

         if (strcmp(request, "help") == 0) {
             help_menu();
             client(inferno_socket);
         }

         if (strcmp(request, "exit") == 0) {
             box_info();
             puts("Exiting Inferno...");
             box_success();
             puts("Exited");
             exit(EXIT_SUCCESS);
         }

         //sending request
         if (send(inferno_socket, request, sizeof(request), 0) == -1)
             error("Error sending command to server");

         //receving response
         char response[1024];
         if (recv(inferno_socket, response, sizeof(response), 0) == -1)
             error("Error receiving instruction from server");
         box_success();
         printf("Result:\n%s\n", response);

         if (strcmp(response, "XXX") == 0) {
             box_success();
             puts("Entering session");
             session(inferno_socket);
         }
    }
    close(inferno_socket);
}

void session(int inferno_socket) {
    while (1) {
        //prompt
        char payload[1024];
        printf("\n[ASX]@[SESSION]> ");
        if (fgets(payload, sizeof(payload), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }
        payload[strcspn(payload, "\n")] = '\0';

        //copying original payload
        char command_copy[1024];
        strcpy(command_copy, payload);

        //parsing payload
        char *command = strtok(payload, " ");
        char *arg1 = strtok(NULL, " ");
        
        //HTTP POST payload
        static char request[1024];
        sprintf(request, "POST /endpoint HTTP/1.0\r\n");
        sprintf(request + strlen(request), "Host: inferno.com\r\n");
        sprintf(request + strlen(request), "Content-Type: application/json\r\n");
        sprintf(request + strlen(request), "Content-Length: %d\r\n", strlen(payload) + 14);
        sprintf(request + strlen(request), "Connection: close\r\n\r\n");
   
        //add arguments to payload if any otherwise send payload
        if (arg1 == NULL) 
            sprintf(request + strlen(request), "{'payload':'%s'}", payload);
        else
            sprintf(request + strlen(request), "{'payload':'%s','argument':'%s'}", payload, arg1);

         box_info();
         printf("Sending %d bytes to server...\n", strlen(payload));

         //sending request
         if (send(inferno_socket, request, sizeof(request), 0) == -1)
             error("Error sending command to server");

         if (strcmp(payload, "exit") == 0)
             client(inferno_socket);

         //receving response
         char response[1024];
         if (recv(inferno_socket, response, sizeof(response), 0) == -1)
             error("Error receiving instruction from server");

         box_info();
         printf("Receiving %d bytes from server...\n", strlen(response));
         box_success();
         printf("Result:\n%s\n", response);
    }
    close(inferno_socket);
}

void banner() {
    printf("\n");
    printf("\033[38;5;208m");
    printf("  _     <-. (`-')_            (`-')  _   (`-') <-. (`-')_                \n");
    printf(" (_)       \\( OO) )  <-.      ( OO).-/<-.(OO )    \\( OO) )     .->       \n");
    printf(" ,-(`-'),--./ ,--/(`-')-----.(,------.,------,),--./ ,--/ (`-')----.     \n");
    printf(" | ( OO)|   \\ |  |(OO|(_\\---' |  .---'|   /`. '|   \\ |  | ( OO).-.  '    \n");
    printf(" |  |  )|  . '|  |)/ |  '--. (|  '--. |  |_.' ||  . '|  |)( _) | |  |    \n");
    printf("(|  |_/ |  |\\    | \\_)  .--'  |  .--' |  .   .'|  |\\    |  \\|  |)|  |    \n");
    printf(" |  |'->|  | \\   |  `|  |_)   |  `---.|  |\\  \\ |  | \\   |   '  '-'  '    \n");
    printf(" `--'   `--'  `--'   `--'     `------'`--' '--'`--'  `--'    `-----'     \n");
    printf("\033[0m");
}

void info() {
    printf("\033[1;31m");
    char info[2048] = "X joined session";
    printf("\nInfo: %s\n", info);
    printf("\033[0m");
}

void help_menu() {
    printf("\nCommand\t\tExample\t\t\tDescription\n");
    printf("=======\t\t=======\t\t\t===========\n\n");
    printf("help\t\t/\t\t\tShow this menu\n");
    printf("exit\t\t/\t\t\tExit Inferno\n");
    printf("listeners\t/\t\t\tlist active listeners\n");
    printf("http\t\thttp 10.129.10.11 8080\tStart an HTTP listener\n");
    printf("stop\t\tstop http 1\t\tStop an HTTP listener with ID X\n");
    printf("sessions\t/\t\t\tList active sessions\n");
    printf("enter\t\tenter 1\t\t\tEnter in session with ID X\n");
    printf("kill\t\tkill 1\t\t\tDestroy session with ID X\n");
}

void box_info() {
    printf("\033[38;5;208m");
    printf("[>] ");
    printf("\033[0m");
}

void box_error() {
    printf("\033[31;5;208m");
    printf("[!] ");
    printf("\033[0m");
}

void box_success() {
    printf("\033[32;5;208m");
    printf("[+] ");
    printf("\033[0m");
}

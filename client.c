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

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

//print the banner
void banner();

//sending infos between operators
void info();

//the networked part
void networking(int inferno_socket);

//client interface
void client(int inferno_socket, char *session_storage);

//session interface
char *session_http(int inferno_socket, char *session_storage);

//help menu
void help_menu();

//choice of actions
void commands(int inferno_socket, char *instruction, int session_ready, char *session_storage);

//transfering to/from server
char *transfer(int inferno_socket, char *instruction);

//a nice small set of info box
void box_info();
void box_error();
void box_success();

int main() {
    char session_storage[1024];
    banner();
    info();

    //initialize socket
    box_info();
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Initializing socket failed");

    networking(inferno_socket);

    client(inferno_socket, session_storage);

    close(inferno_socket);
}

void client(int inferno_socket, char *session_storage) {
    int session_ready = 0;
    while (1) {
        //crafting task
        static char instruction[2048];
        printf("\n[ASX]@[INFERNO]> ");

        if (fgets(instruction, sizeof(instruction), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction[strcspn(instruction, "\n")] = '\0';

        commands(inferno_socket, instruction, session_ready, session_storage);

        char *receive;
        receive = transfer(inferno_socket, instruction);
        
        if (strcmp(receive, "XXX-AGENT-REDGHOST-XXX") == 0)
            strcpy(session_storage, receive);
            session_ready = 1;
            box_success();
            printf("Session: %s is ready!\n", session_storage);
    }
}

void banner() {
    printf("\n");
    // Orange color for arrows
    printf("\033[38;5;208m");
    
    // Print the ASCII art
    printf("  _     <-. (`-')_            (`-')  _   (`-') <-. (`-')_                \n");
    printf(" (_)       \\( OO) )  <-.      ( OO).-/<-.(OO )    \\( OO) )     .->       \n");
    printf(" ,-(`-'),--./ ,--/(`-')-----.(,------.,------,),--./ ,--/ (`-')----.     \n");
    printf(" | ( OO)|   \\ |  |(OO|(_\\---' |  .---'|   /`. '|   \\ |  | ( OO).-.  '    \n");
    printf(" |  |  )|  . '|  |)/ |  '--. (|  '--. |  |_.' ||  . '|  |)( _) | |  |    \n");
    printf("(|  |_/ |  |\\    | \\_)  .--'  |  .--' |  .   .'|  |\\    |  \\|  |)|  |    \n");
    printf(" |  |'->|  | \\   |  `|  |_)   |  `---.|  |\\  \\ |  | \\   |   '  '-'  '    \n");
    printf(" `--'   `--'  `--'   `--'     `------'`--' '--'`--'  `--'    `-----'     \n");
    
    // Reset colors and formatting
    printf("\033[0m");
}

void info() {
    printf("\033[1;31m");
    char info[2048] = "X joined session";
    printf("\nInfo: %s\n", info);
    printf("\033[0m");
}

void networking(int inferno_socket) {
    // crafting all necessary for address
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect
    box_info();
    printf("Connecting...\n");
    if (connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
         error("Error connecting");
    box_success();
    printf("Connected\n");
}

char *session_http(int inferno_socket, char *session_storage) {
    box_info();
    printf("Entering sessionX...\n");

    //sending session status to server
    char *status = "ok";
    if(send(inferno_socket, status, sizeof(status), 0) == -1)
        error("Sending status");
    box_info();
    printf("Sent:\n%s\n", status);

    while (1) {
    	//crafting task
    	static char instruction_payload[2048];
    	printf("[ASX]@[SESSION]> ");

    	if (fgets(instruction_payload, sizeof(instruction_payload), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction_payload[strcspn(instruction_payload, "\n")] = '\0';
    
        //copying original payload
        char command_copy[2048];
        strcpy(command_copy, instruction_payload);

        //parsing payload
        char *command = strtok(instruction_payload, " ");
        char *arg1 = strtok(NULL, " ");
        
        //HTTP POST payload
        static char request[2048];
        sprintf(request, "POST /endpoint HTTP/1.0\r\n");
        sprintf(request + strlen(request), "Host: inferno.com\r\n");
        sprintf(request + strlen(request), "Content-Type: application/json\r\n");
        sprintf(request + strlen(request), "Content-Length: %d\r\n", strlen(instruction_payload) + 14);
        sprintf(request + strlen(request), "Connection: close\r\n\r\n");
   
        //add arguments to payload if any otherwise send payload
        if (arg1 == NULL) 
            sprintf(request + strlen(request), "{'payload':'%s'}", instruction_payload);
        else
            sprintf(request + strlen(request), "{'payload':'%s','argument':'%s'}", instruction_payload, arg1);

        //show payload infos
        box_info();
        printf("Sending %d bytes to server...\n", strlen(instruction_payload));
        
        //send payload to server
        box_info();
        printf("Sending payload to server: %s\n", instruction_payload);
        if (send(inferno_socket, request, sizeof(request), 0) == -1)
            error("Error sending payload to server");

        //exiting session
        if (strcmp(instruction_payload, "exit") == 0) {
            box_info();
            puts("Closing session and exiting...");
            client(inferno_socket, session_storage);
        }

        // wait for response
        char result[2048];
        box_info();
        printf("Waiting for response...\n");
       
        //receiving payload result from server
        if (recv(inferno_socket, result, sizeof(result), 0) == -1)
            error("Error receiving payload result from server");
        
        //infos result
        box_info();
        printf("Receiving %d bytes from server...\n", strlen(result));
        
        //result
        box_success();
        printf("Result:\n%s", result); 
    }
}

void help_menu() {
    printf("\nCommand\t\tDescription\n");
    printf("=======\t\t===========\n\n");
    printf("exit\t\texit Inferno\n");
    printf("help\t\tshow this menu\n");
    printf("http\t\tstart an HTTP listener\n");
    printf("enter\t\tenter in session\n");
    printf("sessions\t\tlist active sessions\n\n");
}

void commands(int inferno_socket, char *instruction, int session_ready, char *session_storage) {
    //list sessions
    if (strcmp(instruction, "sessions") == 0) {
        box_info();
        printf("SESSIONS\n");
        printf("========\n");
        printf("1. %s\n", session_storage);
        client(inferno_socket, session_storage);
    }
    //help menu
    else if (strcmp(instruction, "help") == 0) {
        help_menu();
        client(inferno_socket, session_storage);
    }
    //exiting c2
    else if (strcmp(instruction, "exit") == 0) {
        box_info();
        puts("Closing c2 and exiting...");
        exit(EXIT_SUCCESS);
    }
    //starting HTTP listener
    else if (strcmp(instruction, "http") == 0) {
        char status[1024] = "http";
        box_info();
        puts("Starting listener...");
        box_success();
        printf("Listening on ADDRESS: %s\n\r\t\t    PORT: %d\n", IP, PORT_HTTP);
        if (send(inferno_socket, status, sizeof(status), 0) == -1)
            error("Error sending command to server");
        //client(inferno_socket);
    }
    //entering session
    else if (strncmp(instruction, "enter ", 6) == 0) {
        char *session_name = instruction + 6;
        if (strcmp(session_name, session_storage) == 0) {
            session_http(inferno_socket, session_storage);
        }
    }
    else {
        box_error();
        puts("Wrong command");
        client(inferno_socket, session_storage);
    }
}

char *transfer(int inferno_socket, char *instruction) {
    //sending
    if (send(inferno_socket, instruction, sizeof(instruction), 0) == -1)
        error("Error sending command to server");

    // wait for response
    static char receive[2048];
    box_info();
    printf("Waiting for response...\n");

    //receiving from server
    if (recv(inferno_socket, receive, sizeof(receive), 0) == -1)
        error("Error receiving instruction from server");
    receive[strcspn(receive, "\n")] = '\0';
    box_success();
    printf("Result: %s\n", receive);
    return receive;
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


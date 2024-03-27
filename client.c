#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT 33333
#define PORT_HTTP 80

//print the banner
void banner();

//sending infos between operators
void info();

//the networked part
void networking(int inferno_socket);

//client interface
void client(int inferno_socket);

//session interface
char *session_http(int inferno_socket);

//help menu
void help_menu();

//choice of actions
void commands(int inferno_socket, char *instruction, int session_ready);

//transfering to/from server
char *transfer(int inferno_socket, char *instruction);

//a nice small info box
void box_info();

int main() {
    banner();
    info();

    //initialize socket
    box_info();
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    networking(inferno_socket);

    client(inferno_socket);

    close(inferno_socket);
}

void client(int inferno_socket) {
    // check if session is ready
    int session_ready = 0;

    while (1) {
        //crafting task
        static char instruction[1024];
        printf("\n[ASX]@[INFERNO]> ");

        if (fgets(instruction, sizeof(instruction), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction[strcspn(instruction, "\n")] = '\0';

        commands(inferno_socket, instruction, session_ready);

        char *receive;
        receive = transfer(inferno_socket, instruction);
        
        if (strcmp(receive, "ok") == 0) {
            session_ready = 1;
            box_info();
            printf("SessionX is ready!\n");
        }
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
    char info[1024] = "X joined session";
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
    int inferno_connect = connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    box_info();
    printf("Connected\n");
}

char *session_http(int inferno_socket) {
    box_info();
    printf("Entering sessionX...\n");

    //sending session status to server
    char *status = "ok";
    ssize_t inferno_send = send(inferno_socket, status, sizeof(status), 0);
    box_info();
    printf("Sent:\n%s\n", status);

    while (1) {
    	//crafting task
    	static char instruction_payload[1024];
    	printf("[ASX]@[SESSION]> ");

    	if (fgets(instruction_payload, sizeof(instruction_payload), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction_payload[strcspn(instruction_payload, "\n")] = '\0';
    
        //copying original payload
        char command_copy[1024];
        strcpy(command_copy, instruction_payload);

        //parsing payload
        char *command = strtok(instruction_payload, " ");
        char *arg1 = strtok(NULL, " ");
        
        //exiting session
        if (strcmp(instruction_payload, "exit") == 0) {
            box_info();
            puts("Closing session and exiting...");
            client(inferno_socket);
        }

        //HTTP POST payload
        static char request[1024];
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
        ssize_t inferno_send = send(inferno_socket, request, sizeof(request), 0);

        // wait for response
        char result[1024];
        box_info();
        printf("Waiting for response...\n");
       
        //receiving payload result from server
        int inferno_receive = recv(inferno_socket, result, sizeof(result), 0);
        
        //infos result
        box_info();
        printf("Receiving %d bytes from server...\n", strlen(result));
        
        //result
        box_info();
        printf("Result:\n%s", result); 
    }
}

void help_menu() {
    printf("\nCommand\tDescription\n");
    printf("=======\t===========\n\n");
    printf("exit\texit Inferno\n");
    printf("help\tshow this menu\n");
    printf("http\tstart an HTTP listener\n");
    printf("enter\tenter in session\n\n");
}

void commands(int inferno_socket, char *instruction, int session_ready) {
    //help menu
    if (strcmp(instruction, "help") == 0) {
        help_menu();
    }
    //exiting c2
    else if (strcmp(instruction, "exit") == 0) {
        box_info();
        puts("Closing c2 and exiting...");
        exit(EXIT_SUCCESS);
    }
    //starting HTTP listener
    else if (strcmp(instruction, "http") == 0) {
        box_info();
        puts("Starting listener...");
        box_info();
        printf("Listening on ADDRESS: %s\n\r\t\t    PORT: %d\n", IP, PORT_HTTP);
    }
    //entering session
    else if (strcmp(instruction, "enter") == 0 && session_ready) {
        session_http(inferno_socket);
    }
}

char *transfer(int inferno_socket, char *instruction) {
    //sending
    send(inferno_socket, instruction, sizeof(instruction), 0);

    // wait for response
    static char receive[1024];
    box_info();
    printf("Waiting for response...\n");

    //receiving from server
    recv(inferno_socket, receive, sizeof(receive), 0);
    receive[strcspn(receive, "\n")] = '\0';
    box_info();
    printf("Result: %s\n", receive);
    return receive;
}

void box_info() {
    printf("\033[38;5;208m");
    printf("[>] ");
    printf("\033[0m");
}

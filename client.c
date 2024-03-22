#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 8000

//print the banner
void banner();
//sending infos between operators
void info();
//the networked part
void networking();

int main() {

    banner();
    info();
    networking();
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

void networking() {
    //socket
    printf("\033[38;5;208m");
    printf("[>] ");
    printf("\033[0m");
    printf("Initializing socket...\n");

    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //connect
    printf("\033[38;5;208m");
    printf("[>] ");
    printf("\033[0m"); 
    printf("Connecting...\n");
    int inferno_connect = connect(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address));
    printf("\033[38;5;208m");
    printf("[>] ");
    printf("\033[0m");
    printf("Connected\n");

    while (1) {
        int payload;
        //crafting task
        static char instruction[1024];
        printf("\n[ASX]@[INFERNO]> ");

        if (fgets(instruction, sizeof(instruction), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction[strcspn(instruction, "\n")] = '\0';
    
        if (strcmp(instruction, "exit") == 0) {
            printf("\033[38;5;208m");
            printf("[>] ");
            printf("\033[0m");
            puts("\nClosing c2 and exiting...");
            exit(EXIT_SUCCESS);
        }

        if (strcmp(instruction, "whoami") == 0) {
            payload = 1;
        } else {
            payload = 10;
        }
        
        if (strcmp(instruction, "http") == 0) {
            printf("\033[38;5;208m");
            printf("\n[>] ");
            printf("\033[0m");
            
            puts("Starting listener...");
            printf("\033[38;5;208m");
            printf("[>] ");
            printf("\033[0m");
            puts("Listening");
            
            puts("==============================");
            printf(" IP: %s                  \n", IP);
            puts("==============================");
            printf(" PORT: 80                \n");
            puts("==============================");
            continue;
        }
        
        
        //sending
        char request[1024];
        sprintf(request, "POST /endpoint HTTP/1.0\r\n");
        sprintf(request + strlen(request), "Host: inferno.com\r\n");
        sprintf(request + strlen(request), "Content-Type: application/json\r\n");
        sprintf(request + strlen(request), "Content-Length: %d\r\n", payload);
        sprintf(request + strlen(request), "Connection: close\r\n\r\n");
        sprintf(request + strlen(request), "{'type':'ghost','element':'fire','place':'mansion':'state':'haunted'}");

        ssize_t inferno_send = send(inferno_socket, request, sizeof(request), 0);

        // wait for response
        char receive[1024];

        printf("\033[38;5;208m");
        printf("\n[>] ");
        printf("\033[0m");
        printf("Waiting for response...\n");
        
        int inferno_receive = recv(inferno_socket, receive, sizeof(receive), 0);
        receive[strcspn(receive, "\n")] = '\0';
        
        printf("\033[38;5;208m");
        printf("[>] ");
        printf("\033[0m");
        printf("Result: %s\n", receive);
    }
}

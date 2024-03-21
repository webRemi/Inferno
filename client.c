#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define IP "127.0.0.1"
#define PORT 8000

void banner();
void info();
char *command();
void networking();

int main() {

    banner();
    info();
    networking();

    while (1) {
        command();
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

char *command() {
    static char input[1024];
    printf("%s\n\n", input);
    printf("[ASX]@[INFERNO]> ");

    if (fgets(input, sizeof(input), stdin) == NULL){
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    input[strcspn(input, "\n")] = '\0';
    
    if (strcmp(input, "exit") == 0) {
         puts("\nClosing c2 and exiting...");
         exit(EXIT_SUCCESS);
    }
    return input;
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
    printf("Connected");
    

    while (1) {

    static char input[1024];
    printf("%s\n\n", input);
    printf("[ASX]@[INFERNO]> ");

    if (fgets(input, sizeof(input), stdin) == NULL){
        perror("fgets failed");
        exit(EXIT_FAILURE);
    }

    input[strcspn(input, "\n")] = '\0';
    
    if (strcmp(input, "exit") == 0) {
         puts("\nClosing c2 and exiting...");
         exit(EXIT_SUCCESS);
    }

    //commands
    char *instruction;
    instruction = input;
    puts(instruction);
    
    //sending
    ssize_t inferno_send = send(inferno_socket, instruction, sizeof(instruction), 0);
    }
}

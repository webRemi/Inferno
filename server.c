#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#define IP "127.0.0.1"
#define TCP_PORT 33333
#define MAX_LISTENERS 10
#define MAX_SESSIONS 10

struct http_listener {
    int id;
    char protocol[20];
    char address[20];
    int port;
    int socket;
};

struct http_session {
    int id;
    char protocol[20];
    char address[20];
    int port;
    int http_accept;
};

struct thread_shared_data {
    struct http_listener *listener;
    struct http_session *session;
    struct http_session *selected_session;
    int listener_id;
    int listener_num;
    int session_id;
    int session_num;
    int tcp_socket;
    int tcp_accept;
    int http_socket;
    int http_accept;
};

//handle erros
void error(char *message) {
    fprintf(stderr, "%s: %s\n", message, strerror(errno));
    exit(EXIT_FAILURE);
}

void *accept_tcp_thread(void *args);

void *process_tcp_thread(void *args);

void *accept_http_thread(void *args);

void *process_http_thread(void *args);

void craft_http_listener(char *request, struct http_listener *listener, int listener_id, int listener_num);

int start_http_listener(struct http_listener *listener);

void stop_http_listener(char *request, struct http_listener *listener);

void craft_http_session(struct http_listener *listener, struct http_session *session, int session_id, int session_num);

void destroy_http_session(char *request, struct http_session *session);

int main() {
    struct http_listener listener[MAX_LISTENERS] = {0};
    int listener_id = 1;
    int listener_num = 0;

    struct http_session session[MAX_SESSIONS] = {0};
    int session_id = 1;
    int session_num = 0;

    struct thread_shared_data *args = malloc(sizeof(struct thread_shared_data));
    if (args == NULL)
        error("Error allocating memory");

    //socket tcp
    puts("Initializing socket...");
    int tcp_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (tcp_socket == -1)
        error("Socket creation failed");

    //preparing address tcp
    struct sockaddr_in tcp_address;
    memset(&tcp_address, 0, sizeof(tcp_address));
    tcp_address.sin_family = AF_INET;
    tcp_address.sin_port = htons(TCP_PORT);
    tcp_address.sin_addr.s_addr = inet_addr(IP);

    int tcp_reuse = 1;
    //reuse address tcp
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEADDR, &tcp_reuse, sizeof(tcp_reuse)) == -1)
        error("Error reusing tcp address");

    //reuse port tcp
    if (setsockopt(tcp_socket, SOL_SOCKET, SO_REUSEPORT, &tcp_reuse, sizeof(tcp_reuse)) == -1)
        perror("Error reusing tcp port");

    //bind tcp
    puts("Binding socket to Inferno address...");
    if (bind(tcp_socket, (struct sockaddr *) &tcp_address, sizeof(tcp_address)) == -1)
        error("Binding socket failed");

    //listen tcp
    printf("Listening to %s:%d\n", IP, TCP_PORT);
    if (listen(tcp_socket, 0) == -1)
        error("Listening failed");

   args->listener = listener;
   args->listener_id = listener_id;
   args->listener_num = listener_num;
   args->session = session;
   args->session_id = session_id;
   args->session_num = session_num;
   args->tcp_socket = tcp_socket;

   pthread_t accept_tcp_thread_id;
   pthread_create(&accept_tcp_thread_id, NULL, accept_tcp_thread, (void *)args);

   while(1);

   free(args);
}

void *accept_tcp_thread(void *args) {
    struct thread_shared_data *targs = (struct thread_shared_data *)args;
    int tcp_socket = targs->tcp_socket;

    while(1) {
        struct http_session *session = targs->session;
        struct http_listener *listener = targs->listener;
        int listener_id = targs->listener_id;
        int listener_num = targs->listener_num;
        int session_id = targs->session_id;
        int session_num = targs->session_num;

        //accept tcp
        puts("Accepting client...");
        int tcp_accept = accept(tcp_socket, NULL, NULL);
        if (tcp_accept == -1)
            error("Accepting failed");
        puts("Client connected");

        targs->tcp_accept = tcp_accept;
        targs->listener = listener;
        targs->session = session;
        targs->session_id = session_id;
        targs->session_num = session_num;
        targs->listener_id = listener_id;
        targs->listener_num = listener_num;

        pthread_t process_tcp_thread_id;
        pthread_create(&process_tcp_thread_id, NULL, process_tcp_thread, (void *)args);
    }
    close(tcp_socket);
}

void *process_tcp_thread(void *args) {
    struct thread_shared_data *targs = (struct thread_shared_data *)args;
    int tcp_accept = targs->tcp_accept;

    while (1) {
        if (targs->selected_session != NULL) {
            sleep(1);
            continue;
        }

        struct http_listener *listener = targs->listener;
        int listener_id = targs->listener_id;
        int listener_num = targs->listener_num;
        struct http_session *session = targs->session;
        int session_id = targs->session_id;
        int session_num = targs->session_num;
        int http_accept = targs->http_accept;

        //read request
        char request[1024];
        if (recv(tcp_accept, request, sizeof(request), 0) == -1)
            error("Error receiving request");

        //watch for listeners
        if (strncmp(request, "http ", 5) == 0) {
            if (listener_id == MAX_LISTENERS)
                printf("Sorry no more than %s listeners allowed", MAX_LISTENERS);
            craft_http_listener(request, &listener[listener_num], listener_id, listener_num);
            int http_socket = start_http_listener(&listener[listener_num]); 
            listener_num++;
            listener_id++;

            targs->listener = listener;
            targs->session_id = session_id;
            targs->session_num = session_num;
            targs->session = session;
            targs->listener_id = listener_id;
            targs->listener_num = listener_num;
            targs->http_socket = http_socket;

            pthread_t thread_http;
            pthread_create(&thread_http, NULL, accept_http_thread, (void *)args);
        }

        char response[1024] = "HTTP/1.0 200 OK\r\n\r\n";

        if (strcmp(request, "listeners") == 0) {
            char listeners[1024];
            sprintf(listeners, "ID\tPROTOCOL\tADDRESS\tPORT\n");
            sprintf(listeners + strlen(listeners), "==\t========\t=======\t====\n");
            for (int i = 0; i < listener_num; i++) {
                if (listener[i].id == -1)
                    continue;
                sprintf(listeners + strlen(listeners), "%d\t%s\t\t%s\t%d\n", listener[i].id, listener[i].protocol, listener[i].address, listener[i].port);
            }
            strcpy(response, listeners);
        }

        else if (strcmp(request, "sessions") == 0) {
            int session_num = targs->session_num;
            int session_id = targs->session_id;

            char sessions[1024];
            sprintf(sessions, "ID\tCOMMUNICATION\tREMOTE ADDRESS\n");
            sprintf(sessions + strlen(sessions), "==\t=============\t===========\n");
            printf("session_num outside: %d\n", session_num);
            for (int i = 0; i < session_num; i++) {
                if (session[i].id == -1)
                    continue;
                sprintf(sessions + strlen(sessions), "%d\t%s\t%s\t%d\n", session[i].id, session[i].protocol, session[i].address, session[i].port);
            }
            strcpy(response, sessions);
        }

        else if (strncmp(request, "enter ", 6) == 0) {
            int session_num = targs->session_num;
            char *session_number = strtok(request + 6, " ");
            int session_selected = atoi(session_number);

            struct http_session *selected_session = NULL;
            printf("Debug: session_num: %d\n", session_num);
            for (int i = 0; i < session_num; i++) {
                 if (session[i].id == session_selected) {
                     printf("Debug: Session id: %d\n", session[i].id);
                     printf("Debug: Session_selected: %d\n", session_selected);
                     selected_session = &session[i];
                     printf("Debug: Selected port number: %d\n", selected_session->port);
                     break;
                 }
            }
            targs->selected_session = selected_session;
            if (targs->selected_session != NULL) {
                puts("Debug: selected session is not null");
                targs->selected_session = selected_session;
            }

            strcpy(response, "XXX");

            pthread_t process_http_thread_id;
            pthread_create(&process_http_thread_id, NULL, process_http_thread, (void *)targs);
        }

        else if (strncmp(request, "stop http ", 10) == 0) {
            stop_http_listener(request, listener);
            targs->listener = listener;
        }

        else if (strncmp(request, "kill ", 5) == 0) {
            destroy_http_session(request, session);
            targs->session = session;
        }

        //send response
        if (send(tcp_accept, response, sizeof(response), 0) == -1)
            error("Error sending response");
    }
    close(tcp_accept);
    pthread_exit(NULL);
}


void craft_http_listener(char *request, struct http_listener *listener, int id, int listener_num) {
    char *protocol = strtok(request, " ");
    char *address = strtok(NULL, " ");
    char *port_string = strtok(NULL, "\n");

    listener->id = id;
    strcpy(listener->protocol, protocol);
    strcpy(listener->address, address);
    listener->port = atoi(port_string);
}

int start_http_listener(struct http_listener *listener) {
    //socket http
    puts("Initializing http listener...");
    int http_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (http_socket == -1)
        error("HTTP socket creation failed");

    //preparing http address
    struct sockaddr_in http_address;
    memset(&http_address, 0, sizeof(http_address));
    http_address.sin_family = AF_INET;
    http_address.sin_port = htons(listener->port);
    http_address.sin_addr.s_addr = inet_addr(listener->address);
    puts("HTTP listener initialized");

    //reuse http
    int http_reuse = 1;
    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEADDR, &http_reuse, sizeof(http_reuse)) == -1)
        error("Error reusing http address");

    if (setsockopt(http_socket, SOL_SOCKET, SO_REUSEPORT, &http_reuse, sizeof(http_reuse)) == -1)
        error("Error reusing http port");

    //bind http
    puts("Binding http listener...");
    if (bind(http_socket, (struct sockaddr *) &http_address, sizeof(http_address)) == -1)
        error("Binding HTTP failed");

    //listen http
    puts("Listening...");
    if (listen(http_socket, 0) == -1)
        error("Failed listening");

    listener->socket = http_socket;

    printf("Started listener with socket n*: %d\n", listener->socket);

    return http_socket;
}

void *accept_http_thread(void *args) {
    struct thread_shared_data *targs = (struct thread_shared_data *)args;
    int http_socket = targs->http_socket;

    while (1) {
        struct http_listener *listener = targs->listener;
        int listener_id = targs->listener_id;
        int listener_num = targs->listener_num;
        struct http_session *session = targs->session;
        int session_id = targs->session_id;
        int session_num = targs->session_num;
        int id = targs->listener->id;

        //accept http
        puts("Accepting agent...");
        int http_accept = accept(http_socket, NULL, NULL);
        if (listener[listener_num - 1].id == -1) {
            puts("Listener stoped");
            break;
        }
        int used_listener_id = -1;
        for (int i = 0; i < listener_num; i++) {
            if (http_socket == listener[i].socket) {
                printf("listener socket: %d\n", listener[i].port);
                used_listener_id = listener[i].id;
                //break;
            }
        }
        session_id = targs->session_id;
        session_num = targs->session_num;
        printf("Listener used was: %d\n", used_listener_id);
        printf("Before assigning socket: session_num: %d, session_id: %d, session_num - 1: %d\n", session_num, session_id, session_num - 1);
        craft_http_session(&listener[used_listener_id - 1], &session[session_num], session_id, session_num);
        if (session_id == MAX_SESSIONS)
            printf("Sorry no more than %s sessions allowed", MAX_SESSIONS);
        session_id++;
        session_num++;
        printf("upgraded session number, session_id: %d\n", session_id);
        printf("session_num: %d\n", session_num);

        targs->listener = listener;
        targs->listener_id = listener_id;
        targs->listener_num = listener_num;
        targs->session = session;
        targs->session_id = session_id;
        targs->session_num = session_num;
        targs->http_socket = http_socket;
        targs->session[session_num -1].http_accept = http_accept;
    }
    close(http_socket);
    pthread_exit(NULL);
}

void *process_http_thread(void *args) {
     struct thread_shared_data *targs = (struct thread_shared_data *)args;
     int tcp_accept = targs->tcp_accept;
     int http_accept = targs->selected_session->http_accept;

     while (1) {
         //receive client
         char request_tcp[1024];
         ssize_t receive = recv(tcp_accept, request_tcp, sizeof(request_tcp), 0);
         if (receive == -1)
             error("Error receiving from client");
         else if (receive == 0) {
             error("Client closed the connection");
         }
         printf("Received for agent: %s\n", request_tcp);

         if (strcmp(request_tcp, "exit") == 0) {
             puts("exited");
             targs->selected_session = NULL;
             targs->tcp_accept = tcp_accept;
             break;
         }

         //send agent
         if (send(http_accept, request_tcp, sizeof(request_tcp), 0) == -1)
             error("Error sending to agent");

         //receive agent
         char response_http[1024];
         if (recv(http_accept, response_http, sizeof(response_http), 0) == -1)
             error("Error receiving from agent");

          //send client
          if (send(tcp_accept, response_http, sizeof(response_http), 0) == -1)
              error("Error sending to client");
     }
     //close(http_accept); -> close only if we destroy session
     //pthread_exit(NULL);
}

void craft_http_session(struct http_listener *listener, struct http_session *session, int session_id, int session_num) {
    session->id = session_id;
    strcpy(session->protocol, listener->protocol);
    strcpy(session->address, listener->address);
    session->port = listener->port;
}

void stop_http_listener(char *request, struct http_listener *listener) {
    char *extract = strstr(request, "stop http ");
    extract += strlen("stop http ");
    char *stoped_string = strtok(extract, "\n");
    int stoped_id = atoi(stoped_string);
    printf("Stoped listener n*: %d\n", stoped_id);

    listener[stoped_id - 1].id = -1;
    shutdown(listener[stoped_id - 1].socket, SHUT_RD);
    close(listener[stoped_id - 1].socket);
}

void destroy_http_session(char *request, struct http_session *session) {
    char *extract = strstr(request, "kill ");
    extract += strlen("kill ");
    char *killed_string = strtok(extract, "\n");
    int killed_id = atoi(killed_string);
    printf("Destroyed session n*: %d\n", killed_id);

    session[killed_id - 1].id = -1;
    close(session[killed_id -1].http_accept);
}

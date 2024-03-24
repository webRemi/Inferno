#include <stdio.h>
#include <string.h>

int main() {
    //sending
        char request[1024];
        sprintf(request, "POST /endpoint HTTP/1.0\r\n");
        sprintf(request + strlen(request), "Host: inferno.com\r\n");
        sprintf(request + strlen(request), "Content-Type: application/json\r\n");
        sprintf(request + strlen(request), "Content-Length: %d\r\n", 1);
        sprintf(request + strlen(request), "Connection: close\r\n\r\n");
        sprintf(request + strlen(request), "{'type':'ghost','element':'fire','place':'mansion':'state':'haunted'}");
        puts(request);
}

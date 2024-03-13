#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>

void command(char *instruction, char *argument) {
    char *pathname = instruction;
    char *argv[] = {pathname, argument, NULL};
    char *envp[] = {NULL};
    execve(pathname, argv, envp);
    perror("execve failed");
    exit(EXIT_FAILURE);
}

int main() {
    while (1) {
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        }

        if (child_pid == 0) {
            // child process
            char instruction[100];
            char argument[100];
            printf("> ");
            scanf("%s" "%s", instruction, argument);
            command(instruction, argument);
        } else {
            // parent process
            wait(NULL);
        }
    }

    return 0;
}

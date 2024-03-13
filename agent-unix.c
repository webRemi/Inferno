#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// command execution
void command(char *instruction, char *argument1, char *argument2) {
    printf("Executing: %s %s %s...", instruction, argument1, argument2);
    char *pathname = instruction;
    char *argv[] = {pathname, argument1, argument2, NULL};
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
            char input[200];
            //char instruction[200];
            //char argument[200];
            printf("> ");
            if (fgets(input, sizeof(input), stdin) == NULL){
                perror("fgets failed");
                exit(EXIT_FAILURE);
            }
            input[strcspn(input, "\n")] = '\0';
            char input_copy[200];
            strcpy(input_copy, input);
            
            char *instruction = strtok(input_copy, " ");
	    char *argument1 = strtok(NULL, " ");
            char *argument2 = strtok(NULL, "\n");

            if (instruction == NULL) {
                puts("Failed to parse instruction...");
                exit(EXIT_FAILURE);
            }
            if (argument1 == NULL) {
                argument1 = NULL;
            }
	    if (argument2 == NULL) {
	        argument2 = NULL;
	    }
	    if (strcmp(instruction, "exit") == 0) {
		puts("Exit key pressed...");
                break;
		exit(EXIT_SUCCESS);
	    }
            printf("Instruction: '%s'\nArgument1: '%s'\nArgument2: '%s'\n", instruction, argument1, argument2);
            command(instruction, argument1, argument2);
        } else {
            // parent process
            wait(NULL);
        }
    }
    return 0;
}

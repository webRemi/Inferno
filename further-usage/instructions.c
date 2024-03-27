#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct c2_commands {
    char command1[10];
    char command2[10];
    char command3[10];
    char command4[10];
};

int main() {
    struct c2_commands external;
    external.command1 = "help";
    external.command2 = "exit";
    external.command3 = "http";
    external.command4 = "enter";
    struct c2_commands *ptr = &external;

    while (1) {
        static char instruction[1024];
        printf("\n[ASX]@[INFERNO]> ");

        if (fgets(instruction, sizeof(instruction), stdin) == NULL){
            perror("fgets failed");
            exit(EXIT_FAILURE);
        }

        instruction[strcspn(instruction, "\n")] = '\0';

        switch(strcmp(instruction)) {
            case ptr->command1:
                printf("%s\n", ptr->command1);
                break;
            case ptr->command2:
                printf("%s\n", ptr->command2);
        }
    }

}


#include <unistd.h>
#include <stdio.h>

int main() {
    char directory[1024];
    getcwd(directory, sizeof(directory));
    puts(directory);
}

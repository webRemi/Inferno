#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>

int main() {
    DIR *dirp;
    struct dirent *list;
    dirp = opendir(".");
    puts("Size\tName");
    puts("====\t====");
    while ((list = readdir(dirp)) != NULL) {
        printf("%d\t%s\t\n", list->d_reclen, list->d_name);
    }
}

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>

char *lsCommand(char *path);

int main() {
    char response[1024];
    char *path = "/";
    char *buf = lsCommand(path);
    sprintf(response, "%s", buf);
    printf("%s", response);
}

//ls syscall
char *lsCommand(char *path) {
    static char buf[1024];
    DIR *dirp;
    struct dirent *list;
    dirp = opendir(path);
    sprintf(buf, "Size\tName\n");
    sprintf(buf + strlen(buf), "====\t====\n");
    while ((list = readdir(dirp)) != NULL) {
        sprintf(buf + strlen(buf), "%d\t%s\t\n", list->d_reclen, list->d_name);
    }
    return buf;
}
{'payload':'ls /'}

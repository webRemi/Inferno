#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

char *lsCommand(char *path);

int main() {
    char response[1024];
    char *path = "/home/asx/Inferno";
    char *buf = lsCommand(path);
    sprintf(response, "%s", buf);
    printf("%s", response);
}

//ls syscall
char *lsCommand(char *path) {
    struct stat sb;
    static char buf[1024];
    DIR *dirp;
    struct dirent *list;
    dirp = opendir(path);
    sprintf(buf, "Type\t\tSize\tName\n");
    sprintf(buf + strlen(buf), "====\t\t====\t====\n");
    while ((list = readdir(dirp)) != NULL) {
        char type[1024];
        char pathname[1024];
        sprintf(pathname, "%s/%s", path, list->d_name);
        lstat(pathname, &sb);
        switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  sprintf(type, "block device");            break;
           case S_IFCHR:  sprintf(type, "character device");        break;
           case S_IFDIR:  sprintf(type, "directory");               break;
           case S_IFIFO:  sprintf(type, "FIFO/pipe");               break;
           case S_IFLNK:  sprintf(type, "symlink");                 break;
           case S_IFREG:  sprintf(type, "regular file");            break;
           case S_IFSOCK: sprintf(type, "socket");                  break;
           default:       sprintf(type, "unknown?");                break;
        }
        sprintf(buf + strlen(buf), "%s\t%d\t%s\n", type, list->d_reclen, list->d_name);
    }
    return buf;
}

#include <stdio.h>
#include <sys/stat.h>

int main() {
    struct stat sb;
    char buf[1024];
    char *pathname;
    pathname = ".";
    lstat(pathname, &sb);

    switch (sb.st_mode & S_IFMT) {
           case S_IFBLK:  sprintf(buf, "block device\n");            break;
           case S_IFCHR:  sprintf(buf, "character device\n");        break;
           case S_IFDIR:  sprintf(buf, "directory\n");               break;
           case S_IFIFO:  sprintf(buf, "FIFO/pipe\n");               break;
           case S_IFLNK:  sprintf(buf, "symlink\n");                 break;
           case S_IFREG:  sprintf(buf, "regular file\n");            break;
           case S_IFSOCK: sprintf(buf, "socket\n");                  break;
           default:       sprintf(buf, "unknown?\n");                break;
    }
    printf("File is: %s\n", buf);
}

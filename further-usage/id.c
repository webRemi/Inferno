#include <stdio.h>
#include <unistd.h>

int main() {
    int id = getuid();
    int gid = getgid();
    int sizeGroups = getgroups(0, NULL);
    gid_t groups[sizeGroups];
    getgroups(sizeGroups, groups);
    printf("uid=%d gid=%d groups=", id, gid);
    
    for (int i = 0; i < sizeGroups; i++) {
        printf("%d,", groups[i]);
    }
}

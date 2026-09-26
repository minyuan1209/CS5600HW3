#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    // open (or create) a file for writing
    int fd = open("q2.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open failed");
        exit(1);
    }
    printf("before fork: fd = %d\n", fd);

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {                       // child
        printf("child:  fd = %d\n", fd);
        for (int i = 0; i < 5; i++) {
            char *msg = "child writes\n";
            write(fd, msg, strlen(msg));
            usleep(100);                        // tiny pause so they take turns
        }
    } else {                                    // parent
        printf("parent: fd = %d\n", fd);
        for (int i = 0; i < 5; i++) {
            char *msg = "parent writes\n";
            write(fd, msg, strlen(msg));
            usleep(100);
        }
        wait(NULL);
        close(fd);
    }
    return 0;
}
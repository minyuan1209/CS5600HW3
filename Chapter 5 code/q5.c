#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {                       // child
        printf("child:  my pid is %d\n", (int) getpid());
        int w = wait(NULL);                     // child calls wait()
        printf("child:  wait() returned %d (%s)\n", w, strerror(errno));
    } else {                                    // parent
        int w = wait(NULL);                     // parent calls wait()
        printf("parent: child's pid was %d\n", rc);
        printf("parent: wait() returned %d\n", w);
    }
    return 0;
}
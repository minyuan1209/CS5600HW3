#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {                       // child
        printf("child:  my pid is %d\n", (int) getpid());
        exit(7);                                // exit with code 7
    } else {                                    // parent
        int status;
        int w = waitpid(rc, &status, 0);        // wait for THIS child only
        printf("parent: waitpid() returned %d\n", w);
        if (WIFEXITED(status)) {
            printf("parent: child exited with code %d\n", WEXITSTATUS(status));
        }
    }
    return 0;
}
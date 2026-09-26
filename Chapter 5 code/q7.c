#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {                         // child
        printf("child: before closing stdout\n");
        fflush(stdout);                           // print it now
        close(STDOUT_FILENO);                     // close standard output
        printf("child: after closing stdout\n");  // does this show up?
        fprintf(stderr, "child: (stderr still works)\n");
    } else {                                      // parent
        wait(NULL);
        printf("parent: child is done\n");
    }
    return 0;
}
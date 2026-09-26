#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int x = 100;                       // set before fork()
    printf("before fork: x = %d (pid %d)\n", x, (int) getpid());

    int rc = fork();
    if (rc < 0) {                      // fork failed
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {              // child
        printf("child:  x = %d (pid %d)\n", x, (int) getpid());
        x = 200;
        printf("child:  changed x to %d\n", x);
    } else {                           // parent
        printf("parent: x = %d (pid %d)\n", x, (int) getpid());
        x = 300;
        printf("parent: changed x to %d\n", x);
        wait(NULL);                    // wait for child to finish
        printf("parent: after child exits, x = %d\n", x);
    }
    return 0;
}
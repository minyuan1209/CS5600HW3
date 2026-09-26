#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int p[2];                    // p[0] = read end, p[1] = write end
    if (pipe(p) < 0) {
        perror("pipe failed");
        exit(1);
    }

    int rc = fork();
    if (rc < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc == 0) {        // child
        close(p[0]);             // child only writes
        printf("hello\n");
        fflush(stdout);          // make sure "hello" is printed now
        write(p[1], "x", 1);     // tell the parent "I'm done"
        close(p[1]);
    } else {                     // parent
        close(p[1]);             // parent only reads
        char c;
        read(p[0], &c, 1);       // waits here until the child writes
        printf("goodbye\n");
        close(p[0]);
    }
    return 0;
}
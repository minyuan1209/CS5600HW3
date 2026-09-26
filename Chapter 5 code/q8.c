#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int p[2];                              // p[0] = read end, p[1] = write end
    if (pipe(p) < 0) {
        perror("pipe failed");
        exit(1);
    }

    // ----- child 1: the writer -----
    int rc1 = fork();
    if (rc1 < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc1 == 0) {
        close(p[0]);                       // child 1 doesn't read
        dup2(p[1], STDOUT_FILENO);         // stdout now goes into the pipe
        close(p[1]);
        printf("hello from child 1\n");    // goes into the pipe, not the screen
        printf("this is line 2\n");
        exit(0);
    }

    // ----- child 2: the reader -----
    int rc2 = fork();
    if (rc2 < 0) {
        fprintf(stderr, "fork failed\n");
        exit(1);
    } else if (rc2 == 0) {
        close(p[1]);                       // child 2 doesn't write
        dup2(p[0], STDIN_FILENO);          // stdin now comes from the pipe
        close(p[0]);
        char line[100];
        while (fgets(line, sizeof(line), stdin) != NULL) {   // read from stdin
            printf("child 2 received: %s", line);
        }
        exit(0);
    }

    // ----- parent -----
    close(p[0]);                           // parent doesn't use the pipe
    close(p[1]);
    waitpid(rc1, NULL, 0);
    waitpid(rc2, NULL, 0);
    printf("parent: both children are done\n");
    return 0;
}
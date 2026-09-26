#define _GNU_SOURCE              // needed for execvpe() on Linux
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

extern char **environ;           // the current environment variables

int main(void) {
    char *names[] = {"execl", "execle", "execlp", "execv", "execvp", "execvpe"};
    char *args[]  = {"ls", "-l", NULL};   // argument list for the "v" versions
    char *env[]   = {NULL};               // empty environment for the "e" versions

    for (int i = 0; i < 6; i++) {
        printf("\n===== %s =====\n", names[i]);
        fflush(stdout);

        int rc = fork();
        if (rc < 0) {
            fprintf(stderr, "fork failed\n");
            exit(1);
        } else if (rc == 0) {                 // child: replace itself with ls
            switch (i) {
            case 0: execl("/bin/ls", "ls", "-l", NULL);                 break;
            case 1: execle("/bin/ls", "ls", "-l", NULL, env);           break;
            case 2: execlp("ls", "ls", "-l", NULL);                     break;
            case 3: execv("/bin/ls", args);                             break;
            case 4: execvp("ls", args);                                 break;
            case 5:
#ifdef __linux__
                execvpe("ls", args, environ);
#else
                printf("execvpe() is not available on macOS\n");
                exit(0);
#endif
                break;
            }
            perror("exec failed");            // only runs if exec failed
            exit(1);
        } else {                              // parent
            wait(NULL);                       // wait for this child to finish
        }
    }
    return 0;
}
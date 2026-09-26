// ctxswitch.c — estimate context-switch cost, lmbench-style, with two pipes.
// Build: gcc -O2 -o ctxswitch ctxswitch.c
// Run:   ./ctxswitch [round_trips] [cpu]
//
// Parent writes 1 byte to pipe1 and blocks reading pipe2.
// Child blocks reading pipe1, then writes 1 byte to pipe2.
// Each round trip = 2 context switches (parent->child, child->parent).
// Both processes are pinned to the SAME CPU so the OS really has to switch.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>
#ifdef __linux__
#include <sched.h>
#endif

static double now_us(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1e6 + tv.tv_usec;
}

static void pin_to_cpu(int cpu) {
#ifdef __linux__
    cpu_set_t set;
    CPU_ZERO(&set);
    CPU_SET(cpu, &set);
    if (sched_setaffinity(0, sizeof(set), &set) != 0) {
        perror("sched_setaffinity");
        exit(1);
    }
#else
    (void)cpu;   // macOS has no sched_setaffinity; results are less reliable
#endif
}

int main(int argc, char *argv[]) {
    long iters = (argc > 1) ? atol(argv[1]) : 100000;
    int cpu    = (argc > 2) ? atoi(argv[2]) : 0;
    char c = 'x';

    pin_to_cpu(cpu);   // child inherits this affinity after fork()

    // ---- Baseline: pipe write+read cost in ONE process (no switch) ----
    int p[2];
    if (pipe(p) < 0) { perror("pipe"); return 1; }
    double b0 = now_us();
    for (long i = 0; i < iters; i++) {
        write(p[1], &c, 1);
        read(p[0], &c, 1);
    }
    double b1 = now_us();
    close(p[0]); close(p[1]);
    // One round trip does this write+read pair twice (once per process)
    double pipe_cost_us = (b1 - b0) / iters;          // per write+read pair

    // ---- Ping-pong between two processes ----
    int p1[2], p2[2];
    if (pipe(p1) < 0 || pipe(p2) < 0) { perror("pipe"); return 1; }

    pid_t pid = fork();
    if (pid < 0) { perror("fork"); return 1; }

    if (pid == 0) {                  // child: read pipe1, write pipe2
        close(p1[1]); close(p2[0]);
        for (long i = 0; i < iters; i++) {
            read(p1[0], &c, 1);
            write(p2[1], &c, 1);
        }
        exit(0);
    }

    // parent: write pipe1, read pipe2
    close(p1[0]); close(p2[1]);
    double t0 = now_us();
    for (long i = 0; i < iters; i++) {
        write(p1[1], &c, 1);
        read(p2[0], &c, 1);
    }
    double t1 = now_us();
    wait(NULL);

    double round_trip_us = (t1 - t0) / iters;
    double per_switch_raw = round_trip_us / 2.0;
    double per_switch_net = (round_trip_us - 2 * pipe_cost_us) / 2.0;

    printf("Pinned to CPU %d, %ld round trips\n", cpu, iters);
    printf("  round trip (2 switches)        = %.2f us\n", round_trip_us);
    printf("  pipe write+read overhead       = %.2f us\n", pipe_cost_us);
    printf("  context switch (raw)           = %.2f us\n", per_switch_raw);
    printf("  context switch (minus pipe I/O)= %.2f us\n", per_switch_net);
    return 0;
}

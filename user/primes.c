// primes printing pipeline
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

void sieve(int lpipe[2]) {
    int p;
    close(lpipe[1]);
    if(read(lpipe[0], &p, 4) != 4) {
        close(lpipe[0]);
        return;
    }

    int rpipe[2];
    pipe(rpipe);
    printf("prime %d\n", p);

    int n;
    if (fork() == 0) {
        close(lpipe[0]);
        close(rpipe[1]);
        sieve(rpipe);
        exit(0);
    }
    else {
        close(rpipe[0]);

        while (read(lpipe[0], &n, 4) == 4) {
            if (n % p != 0) {
                write(rpipe[1], &n, 4);
            }
        }
        close(lpipe[0]);
        close(rpipe[1]);
        wait(0);
        return;
    }
}

int main(int argc, char *argv[]) {
    if(argc != 1) {
        fprintf(2, "usage: primes\n");
        exit(1);
    }

    int pipec[2];
    pipe(pipec);

    int pid = fork();

    if(pid == -1) {
        fprintf(2, "error during fork");
        exit(1);
    }
    else if (pid == 0) {
        sieve(pipec);
        exit(0);
    }
    else {
        close(pipec[0]);
        for (int p = 2; p < 36; p++) {
            write(pipec[1], &p, 4);
        } 
        close(pipec[1]);
        wait(0);
    }

    exit(0);
}
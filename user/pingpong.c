// ping pong between 2 processes
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fcntl.h"

int main(int argc, char *argv[]) {
    if(argc != 1) {
        fprintf(2, "usage: pingpong\n");
        exit(1);
    }

    // fd[0] = read, fd[1] = write
    int ping[2], pong[2];
    pipe(ping);
    pipe(pong);

    int parent = getpid();
    int child = fork();

    if (child  == -1) {
        fprintf(2, "error during fork");
        exit(1);
    }
    else if (child != 0) {
        close(ping[0]);
        int buf[1];
        buf[0] = parent;
        write(ping[1], buf, 4);
        close(ping[1]);
        wait(&child);

        close(pong[1]);
        read(pong[0], buf, 4);
        close(pong[0]);

        printf("%d: received pong\n", buf[0]);
    }
    else {
        close(ping[1]);
        int buf[1];
        read(ping[0], buf, 4);

        printf("%d: received ping\n", buf[0]);
        close(ping[1]);

        close(pong[0]);

        buf[0] = getpid();
        write(pong[1], buf, 4);
        close(pong[1]);

        exit(0);
    }

    exit(0);
}

// standard sleep (ticks)
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int is_valid_integer(const char *str) {
    if (*str == '\0') {
        return 0;
    }
    
    for(int i = 0; str[i] != '\0'; i++) {
        if (str[i] < 48 || str[i] > 57) {
            return 0;
        }
    }

    return 1;
}

int main(int argc, char *argv[]) {
    if(argc != 2) {
        fprintf(2, "usage: sleep {amount}\n");
        exit(1);
    }

    if(is_valid_integer(argv[1]) == 0) {
        fprintf(2, "sleep amount must be a positive integer\n");
        exit(1);
    }

    int ticks = atoi(argv[1]);

    pause(ticks);

    exit(0);
}
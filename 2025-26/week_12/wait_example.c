#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <err.h>
#include <sys/wait.h>

int main(void) {

    for (int i = 0; i < 3; i++) {
    
        pid_t child = fork();
        if (child == 0) {
            // not on exams
        //    printf("C%d\n", i + 1);
        //    exit(0);
            err(243, "Failed");
        }

    }

    for (int i = 0; i < 3; i++) {
        int w_status;
        if (wait(&w_status) < 0) {
            err(1, "Failed to wait");
        }
        printf("%d\n", WEXITSTATUS(w_status));
    }

    printf("P\n");

}

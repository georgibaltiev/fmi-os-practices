#include <unistd.h>
#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

int wrapped_write(int fd, const void* buff, size_t size) {
    
    int w = write(fd, buff, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }

    return w;
}

int main(void) {
	
    pid_t child = fork();

    if (child < 0) {
        err(1, "Failed to create a child process!");
    }

    if (child == 0) {
      
        for (int i = 0; i < 100; i++) {
            wrapped_write(1, "Child\n", strlen("Child\n"));
        }

    }

    for (int i = 0; i < 100; i++) {
        wrapped_write(1, "Parent\n", strlen("Parent\n"));
    }

    int w_status;

    if (wait(&w_status) < 0) {
        err(1, "Failed to wait for child");
    }

    if (WIFEXITED(w_status) == 0) {
        errx(1, "Child terminated abnormally!");      
    }

    exit(0);
}

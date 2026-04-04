#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <string.h>

int wrapped_read(int fd, void* buff, size_t size) {
    
    int r = read(fd, buff, size);

    if (r < 0) {
        err(1, "Failed to read from %d", fd);
    }

    return r;

}

int wrapped_write(int fd, const void* buff, size_t size) {

    int w = write(fd, buff, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }

    return w;

}

int main(void) {
	
    char comm[4096];

    while (true) {
        
        wrapped_write(1, "Write a command:\n", strlen("Write a command:\n"));
        
        int r = wrapped_read(0, comm, sizeof(comm));

        comm[r - 1] = '\0';

        if (!strcmp("exit", comm)) {
            break;
        }

        pid_t child = fork();

        if (child < 0) {
            err(1, "Failed to work a child!");
        }

        if (child == 0) {
            execlp(comm, comm, (char*) NULL);
            err(1, "Failed to execute comm %s", comm);
        }
       
        int w_status;

        if (wait(&w_status) < 0) {
            err(1, "Failed to wait for process!");
        }

        if (!WIFEXITED(w_status)) {
            err(1, "Command %s terminated abnormally!", comm);
        }

    }

}

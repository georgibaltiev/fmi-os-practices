#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <err.h>

pid_t wrapped_fork(void) {

    pid_t child = fork();
    
    if (child < 0) {
        err(1, "Failed to fork a child");
    }

    return child;
}

int wrapped_write(int fd, const void* buff, size_t size) {

    int w = write(fd, buff, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }
    
    return w;
}

int wrapped_read(int fd, void* buff, size_t size) {
    
    int r = read(fd, buff, size);

    if (r < 0) {
        err(1, "Failed to read from %d", fd);
    }

    return r;
}

int wrapped_open(const char* filename, int mode, int* access) {

    int fd;

    if (access) {
        fd = open(filename, mode, *access);
    } else {
        fd = open(filename, mode);
    }
    
    return fd;
}

int wrapped_wait(void) {

    int w_status;
    
    if (wait(&w_status) < 0) {
        err(1, "Failed to wait for child to finish");
    }

    if (!WIFEXITED(w_status)) {
        errx(1, "Child exited abnormally!");
    }
    
    return w_status;
}

int wrapped_pipe(int pipefd[2]) {

    int p = pipe(pipefd);

    if (p < 0) {
        err(1, "Failed to pipe");
    }

    return p;
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "args");
    }

    int pipefd[2];
    wrapped_pipe(pipefd);

    pid_t child = wrapped_fork();

    if (child == 0) {
        close(pipefd[1]);

        char buffer[4096];
        int r;
        while ((r = wrapped_read(pipefd[0], buffer, sizeof(buffer))) > 0) {
            
            int w = wrapped_write(1, buffer, r);
            
            if (w != r) {
                errx(1, "Failed to write exact amount of bytes read from pipe");
            }
        }

        close(pipefd[0]);
        exit(0);
    }

    close(pipefd[0]);
    wrapped_write(pipefd[1], argv[1], strlen(argv[1]));
    close(pipefd[1]);

    wrapped_wait();
}

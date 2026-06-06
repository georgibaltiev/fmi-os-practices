#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int wrapped_open(const char* filename, int mode, int* perm) {
    
    int fd;

    if (perm) {
        fd = open(filename, mode, *perm);
    } else {
        fd = open(filename, mode);
    }
    
    if (fd < 0) {
        err(1, "Failed to open file %s", filename);
    }

    return fd;
}

int wrapped_read(int fd, void* buff, size_t buff_size) {
    
    int r = read(fd, buff, buff_size);
    if (r < 0) {
        err(2, "Failed to read %ld bytes from fd %d", buff_size, fd);
    }

    return r;
}

int wrapped_write(int fd, const void* buff, size_t buff_size) {

    int w = write(fd, buff, buff_size);
    if (w < 0) {
        err(3, "Failed to write %ld bytes from fd %d", buff_size, fd);
    }

    return w;
}

int wrapped_wait(int* w_status) {

    int w = wait(w_status);
    if (w < 0) {
        err(9, "Failed to wait");
    }

    return w;
}

off_t wrapped_lseek(int fd, off_t offset, int whence) {

    off_t l = lseek(fd, offset, whence);
    if (l < 0) {
        err(4, "Failed to lseek at %ld in fd %d", l, fd);
    }

    return l;
}

pid_t wrapped_fork(void) {

    pid_t child = fork();
    if (child < 0) {
        err(5, "Failed to fork");
    }

    return child;
}

int wrapped_pipe(int pipefd[2]) {

    int pfd = pipe(pipefd);
    if (pfd < 0) {
        err(7, "Failed to pipe");
    }

    return pfd;
}

int wrapped_dup2(int oldfd, int newfd) {

    int d = dup2(oldfd, newfd);
    if (d < 0) {
        err(8, "Failed to dup2");
    }
    
    return d;
}

const char* ding = "DING ";
const char* dong = "DONG\n";

int main(int argc, char* argv[]) {

    if (argc != 3) {
        errx(1, "Args count!");
    }

    int n = strtol(argv[1], (void*) NULL, 10);
    int d = strtol(argv[2], (void*) NULL, 10);

    
    int p_to_c[2];
    int c_to_p[2];
    
    wrapped_pipe(p_to_c);
    wrapped_pipe(c_to_p);

    pid_t child = wrapped_fork();

    if (child == 0) {

        close(p_to_c[1]);
        close(c_to_p[0]);

        char buf = 0x00;
        while(wrapped_read(p_to_c[0], &buf, sizeof(buf))) {
            
            wrapped_write(1, dong, strlen(dong));

            wrapped_write(c_to_p[1], &buf, sizeof(buf));
        }

        close(p_to_c[0]);
        close(c_to_p[1]);
    
        exit(0);
    }

    close(p_to_c[0]);
    close(c_to_p[1]);

    char buf = 0x00;

    for (int i = 0; i < n; i++) {
        wrapped_write(1, ding, strlen(ding));
        
        wrapped_write(p_to_c[1], &buf, sizeof(buf));
        wrapped_read(c_to_p[0], &buf, sizeof(buf));
    
        sleep(d);
    }

    close(p_to_c[1]);
    close(c_to_p[0]);
    
    int w_status;
    wrapped_wait(&w_status);

    if (!WIFEXITED(w_status)) {
        err(1, "Child exited abnormally");
    }

    exit(0);
}


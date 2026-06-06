#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
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

int wrapped_wait(int* w_status) {

    int w = wait(w_status);
    if (w < 0) {
        err(9, "Failed to wait");
    }

    return w;
}

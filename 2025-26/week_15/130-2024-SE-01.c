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

int main(int argc, char* argv[]) {

    if (argc != 4) {
        errx(1, "Args");
    }

    long attempts = strtol(argv[2], NULL, 10);

    if (attempts < 0 || attempts > 256) {
        errx(1, "Invalid retries");
    }

    int random_fd = wrapped_open("/dev/urandom", O_RDONLY, NULL);
    
    int perm = S_IRUSR | S_IWUSR;
    int res_fd = wrapped_open(argv[3], O_RDWR | O_TRUNC | O_CREAT, &perm);

    for (long i = 0; i < attempts; i++) {
        
        int pipe_fd[2];
        wrapped_pipe(pipe_fd);

        pid_t child = wrapped_fork();
        if (child == 0) {
            close(pipe_fd[1]);    
            
            wrapped_dup2(pipe_fd[0], 0);
            close(pipe_fd[0]);

            int null_fd = wrapped_open("/dev/null", O_WRONLY, NULL);
            wrapped_dup2(null_fd, 1);
            wrapped_dup2(null_fd, 2);
            close(null_fd);

            execlp(argv[1], argv[1], (char*) NULL);
            err(1, "Failed to exec %s", argv[1]);
        }

        close(pipe_fd[0]);

        uint16_t s;
        wrapped_read(random_fd, &s, sizeof(s));

        char buffer[65535]; 
        memset(buffer, 0, 65535);

        wrapped_read(random_fd, buffer, s);
        wrapped_write(pipe_fd[1], buffer, s);

        close(pipe_fd[1]);
    
        int w_status;
        wrapped_wait(&w_status);
        
        if (WIFSIGNALED(w_status)) {
            wrapped_write(res_fd, buffer, s);
            close(res_fd);
            exit(42);
        }

    }

    close(res_fd);
    close(random_fd);
    exit(0);
}

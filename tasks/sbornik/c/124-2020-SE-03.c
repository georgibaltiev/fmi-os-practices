#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

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
        err(1, "Child exited abnormally!");
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

int wrapped_dup2(int oldfd, int newfd) {
    
    int d = dup2(oldfd, newfd);

    if (d < 0) {
        err(1, "Failed to duplicate %d to %d", oldfd, newfd);
    }

    return d;
}

off_t get_file_size(int fd) {

    struct stat s;

    if (fstat(fd, &s) < 0) {
        err(1, "Failed to stat fd %d", fd);
    }
    
    return s.st_size;
}

typedef struct {
    char name[8];
    uint32_t offs;
    uint32_t len;
} entry;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "Expected 1 parameter!");
    }

    int file_fd = wrapped_open(argv[1], O_RDONLY, NULL);

    off_t file_size = get_file_size(file_fd);
    
    // добавена проверка за размер на индекс файла
    if (file_size % sizeof(entry)) {
        errx(1, "Incorrect file size for %s", argv[1]);
    }

    int r;
    int children = 0;
    entry e;
    
    int pipefd[2];
    wrapped_pipe(pipefd);

    while((r = wrapped_read(file_fd, &e, sizeof(e))) > 0) {
    
        pid_t child = wrapped_fork();
        children += 1;

        if (child == 0) {
            
            close(pipefd[0]);

            int fd = wrapped_open(e.name, O_RDONLY, NULL);
            
            off_t fd_size = get_file_size(fd);

            if ((unsigned long int) fd_size < (e.offs + e.len) * sizeof(uint16_t)) {
                errx(1, "File %s has incorrect size!", e.name);
            }

            off_t off = lseek(fd, e.offs * sizeof(uint16_t), SEEK_SET);
            if (off < 0) {
                err(1, "lseek");
            }

            uint16_t number;
            uint16_t result = 0x0000;

            for (uint32_t i = 0; i < e.len; i++) {
                wrapped_read(fd, &number, sizeof(number));
                result ^= number;
            }

            wrapped_write(pipefd[1], &result, sizeof(result));
        
            close(pipefd[1]);
            close(fd);
            exit(0);
        }
    }

    close(pipefd[1]);

    uint16_t result = 0x0000;
    uint16_t num;

    while((r = wrapped_read(pipefd[0], &num, sizeof(num))) > 0) {
        result ^= num;
    }

    close(pipefd[0]);
    
    // записваме получения резултат на STDOUT
    char buff[8];

    snprintf(buff, 8, "%04X\n", result);

    wrapped_write(1, "result: ", strlen("result: "));
    wrapped_write(1, buff, 8); 
    
    for(int i = 0; i < children; i++) {
        wrapped_wait();
    }

    close(file_fd);
    exit(0);
}

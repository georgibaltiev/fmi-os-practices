#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <stdio.h>

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

    if (argc != 2) {
        errx(1, "Params");
    }

    int find_to_p[2];
    wrapped_pipe(find_to_p);

    pid_t find_child = wrapped_fork();
    if (find_child == 0) {
        close(find_to_p[0]);
        wrapped_dup2(find_to_p[1], 1);
        close(find_to_p[1]);
        
        execlp("find", "find", argv[1], "-type", "f", "-not", "-name", "*.hash", "-print0", (char*) NULL);
        err(1, "Failed to exec find");
    }

    close(find_to_p[1]);
    char filename[4096] = {0};
    char buff;
    int r;
    int index = 0;
    int files_count = 0;

    while ((r = wrapped_read(find_to_p[0], &buff, sizeof(buff))) > 0) {
        if (buff == 0x00) {
            files_count++;
            
            pid_t file_child = wrapped_fork();
           
            printf("%s\n", filename);

            if (file_child == 0) {
                char filename_copy[4096] = {0};
                strcpy(filename_copy, filename);               

                filename_copy[index] = '.';
                filename_copy[index + 1] = 'h';
                filename_copy[index + 2] = 'a';
                filename_copy[index + 3] = 's';
                filename_copy[index + 4] = 'h';
                
                printf("%s %s\n", filename, filename_copy);
                
                int perm = S_IRUSR | S_IWUSR;
                int hash_fd = wrapped_open(filename_copy, O_WRONLY | O_CREAT | O_TRUNC, &perm);
                wrapped_dup2(hash_fd, 1);
                close(hash_fd);

                execlp("md5sum", "md5sum", filename, (char*) NULL);
                err(4, "Failed to call md5sum on %s", filename);
            }

            index = 0;
            memset(filename, 0, sizeof(filename));
        } else {
            filename[index] = buff;
            index++;
        }
    }

    close(find_to_p[0]);
    
    for (int i = 0; i <= files_count; i++) {
        int w_status;
        wrapped_wait(&w_status);

        if (!WIFEXITED(w_status)) {
            errx(1, "Child terminated abnormally");
        }
    }

    exit(0);
}

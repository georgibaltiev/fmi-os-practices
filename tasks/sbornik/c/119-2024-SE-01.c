#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <stdio.h>

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
        err(2, "Failed to write to %d", fd);
    }
    
    return w;
}

int wrapped_read(int fd, void* buff, size_t size) {
    
    int r = read(fd, buff, size);

    if (r < 0) {
        err(3, "Failed to read from %d", fd);
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
   
    if (fd < 0) {
        err(4, "Failed to open %s", filename);
    }

    return fd;
}

int wrapped_wait(void) {

    int w_status;
    
    if (wait(&w_status) < 0) {
        err(5, "Failed to wait for child to finish");
    }

    return w_status;
}

int wrapped_pipe(int pipefd[2]) {

    int p = pipe(pipefd);

    if (p < 0) {
        err(6, "Failed to pipe");
    }

    return p;

}

int wrapped_dup2(int oldfd, int newfd) {
    
    int d = dup2(oldfd, newfd);

    if (d < 0) {
        err(7, "Failed to duplicate %d to %d", oldfd, newfd);
    }

    return d;
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        errx(8, "Insufficient amount of parameters!");
    }

    int n = atoi(argv[2]);

    if (n == 0) {
        errx(9, "Invalid value for num - %s", argv[2]);
    }

    int access = S_IRUSR | S_IWUSR;
    int out_fd = wrapped_open(argv[3], O_CREAT | O_RDWR | O_TRUNC, &access);
    int random_fd = wrapped_open("/dev/urandom", O_RDONLY, NULL);
    int null_fd = wrapped_open("/dev/null", O_WRONLY, NULL);

    for (int i = 0; i < n; i++) {

        int pipefd[2];
        wrapped_pipe(pipefd);
 
        uint16_t input_size;
        wrapped_read(random_fd, &input_size, sizeof(input_size));

        char buff[65536];

        wrapped_read(random_fd, (void*) buff, input_size);

        for (int j = 0; j < input_size; j++) {
            wrapped_write(pipefd[1], buff + j, sizeof(char));
        }

      pid_t fuzzer_child = wrapped_fork();

        if (fuzzer_child == 0) {
            close(out_fd);
            close(random_fd);
            
            close(pipefd[1]);
            
            wrapped_dup2(pipefd[0], STDIN_FILENO);
            close(pipefd[0]);
       
            wrapped_dup2(null_fd, STDOUT_FILENO);
            wrapped_dup2(null_fd, STDERR_FILENO);
            close(null_fd);
           
            execlp(argv[1], argv[1], (const char*) NULL);
            err(10, "Failed to execute %s", argv[1]);
        }

        close(pipefd[0]);
        

        
        close(pipefd[1]);
        
        int w_status = wrapped_wait();

        if (WIFSIGNALED(w_status)) {
            wrapped_write(out_fd, buff, input_size);
            exit(42);
        }

        // Проверка в случай на провален exec
        if (WIFEXITED(w_status) && WEXITSTATUS(w_status) == 10) {
            errx(1, "Child exited abnormally!");
        }

    }

    close(null_fd);
    close(out_fd);
    exit(0);
}

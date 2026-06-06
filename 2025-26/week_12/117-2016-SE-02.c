#include <unistd.h>
#include <sys/wait.h>
#include <err.h>
#include <string.h>

const char* prompt = "Enter your command:\n";

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

pid_t wrapped_fork(void) {

    pid_t child = fork();
    if (child < 0) {
        err(4, "Failed to fork!");
    }

    return child;
}

int main(void) {

    char comm[4096] = { '\0' };    
    char filename[4096] = { '\0' };
   
while(1) {

    memset(comm, '\0', sizeof(comm));
    memset(filename, '\0', sizeof(filename));
    strcat(filename, "/bin/");
    
    wrapped_write(1, prompt, strlen(prompt));
    int r = wrapped_read(0, comm, sizeof(comm));
    comm[r - 1] = '\0';

    if (!strcmp(comm, "exit")) {
        return 0;
    }

    strcat(filename, comm);
    
    pid_t child = wrapped_fork();
    if (child == 0) {
        execl(filename, comm, (char*) NULL);
        err(6, "Failed to execute command %s", comm);
    }

    int w_status;
    int w = wait(&w_status);
    if (w < 0) {
        err(7, "Failed to wait for process");
    }

    if (!WIFEXITED(w_status)) {
        err(8, "Command %s terminated abnormally!", comm);
    }

}
   
}


#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

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
        err(1, "Failed to duplicate %d in %d", newfd, oldfd);
    }

    return d;
}

size_t wrapped_read(int fd, void* buffer, size_t size) {

    int r = read(fd, buffer, size);

    if (r < 0) {
        err(1, "Failed to read from %d", fd);
    }
    
    return r;
}

size_t wrapped_write(int fd, const void* buffer, size_t size) {

    int w = write(fd, buffer, size);

    if (w < 0) {
        err(1, "Failed to read from %d", fd);
    }
    
    return w;
}

pid_t wrapped_fork(void) {
    
    pid_t child = fork();
    
    if (child < 0) {
        err(1, "Failed to fork!");
    }

    return child;
}

const char* ding = "DING\n";
const char* dong = "DONG\n";

int main(int argc, char* argv[]) {

    if (argc != 3) {
        errx(1, "Invalid amount of params!");
    }

    uint8_t n = argv[1][0] - '0';
    uint8_t d = argv[2][0] - '0';

    int parent_to_child[2];
    int child_to_parent[2];

    wrapped_pipe(parent_to_child);
    wrapped_pipe(child_to_parent);

    pid_t child = wrapped_fork();

    if (child == 0) {
        close(parent_to_child[1]);
        close(child_to_parent[0]);
        
        char buff;

        while(read(parent_to_child[0], &buff, 1) > 0) {
            wrapped_write(1, dong, strlen(dong));
            wrapped_write(child_to_parent[1], "c", 1);
        }

        close(parent_to_child[0]);
        close(child_to_parent[1]);
        exit(0);
    }
    close(child_to_parent[1]);
    close(parent_to_child[0]);

    char buff;

    for (int i = 0; i < n; i++) {
        wrapped_write(1, ding, strlen(ding));
        wrapped_write(parent_to_child[1], "c", 1);
        wrapped_read(child_to_parent[0], &buff, 1);
        sleep(d);    
    }
    
    close(parent_to_child[1]);
    close(child_to_parent[0]);
}

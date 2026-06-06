#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main(void) {

    int fd = open("../jorypipe", O_RDONLY);
    if (fd < 0) {
        err(1, "Failed to open the pipe for reading");
    }
    
    if (unlink("../jorypipe") < 0) {
        err(2, "Failed to unlink file jorypipe");
    }

    if (dup2(fd, 0) < 0) {
        err(3, "Failed to dup 0 to %d", fd);
    }

    execlp("sort", "sort", NULL);
    err(4, "Failed to exec sort");
}

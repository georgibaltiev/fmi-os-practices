#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <err.h>

int main(void) {

    int m = mkfifo("../jorypipe", S_IRUSR | S_IWUSR);
    if (m < 0) {
        err(1, "Failed to create a pipe");
    }

    int fd = open("../jorypipe", O_WRONLY);
    if (fd < 0) {
        err(1, "Failed to open");
    }

    if (dup2(fd, 1) < 0) {
        err(1, "Failed to dup %d to 1", fd);
    }

    execlp("cat", "cat", "/etc/passwd", NULL);
    err(1, "Failed to exec cat");
}

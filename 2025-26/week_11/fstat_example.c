#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <sys/stat.h>

int main(void) {

    int fd = open("/etc/passwd", O_RDONLY);
    struct stat st;

    int f = fstat(fd, &st);
    if (f < 0) {
        err(1, "Failed to stat /etc/passwd");
    }

    printf("%ld", st.st_size);

    close(fd);
}

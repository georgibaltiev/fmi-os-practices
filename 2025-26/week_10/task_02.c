#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "Expected 1 param but received %d!", argc - 1);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "Failed to read from %s!", argv[1]);
    }

    int r;
    char c;
    int line_counter = 0;
    
    while ((r = read(fd, &c, sizeof(c))) > 0) {
        int w = write(1, &c, sizeof(c));
        if (w < 0) {
            err(1, "Failed to");
        }

        if (c == '\n') {
            line_counter++;
        }

        if (line_counter == 10) {
            break;
        }
    }

    if (r < 0) {
        err(3, "Failed to read from STDIN!");
    }

    close(fd);
    exit(0);
}

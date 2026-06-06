#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(1, "Expected at most one argument!");
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        err(1, "Failed to open %s", argv[1]);
    }

    int r;
    char buff;
    char num[10];
    int c = 0;
    while ((r = read(fd, &buff, sizeof(buff))) > 0) {
        if (r < 0) {
            close(fd);
            err(1, "Failed to read from %s", argv[1]);
        }
        

        if (buff == '\n') {
            c++;
        }
    }

    if (snprintf(num, 10, "%d\n", c) < 0) {
        close(fd);
        err(1, "Failed to snprintf");
    }

    if (write(1, num, strlen(num)) < 0) {
        close(fd);   
        err(1, "Failed to write to STDOUT");
    }
    
    close(fd);
    return 42;
}

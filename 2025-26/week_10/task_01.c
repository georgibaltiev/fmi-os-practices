#include <fcntl.h>
#include <unistd.h>
#include <err.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        errx(1, "Expected 2 params but received %d!", argc - 1);
    }

    int fd1 = open(argv[1], O_RDONLY);
    if (fd1 < 0) {
        err(1, "Failed to open file %s for reading!", argv[1]);
    }

    int fd2 = open(argv[2], O_TRUNC | O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if (fd2 < 0) {
        err(2, "Failed to open file %s for writing!", argv[2]);
    }

    int r;
    char buff[4096];

    while ((r = read(fd1, buff, sizeof(buff))) > 0) {
        int w = write(fd2, buff, r);
        if (w < 0) {
            err(4, "Failed to write to file %s", argv[2]);
        }
    }

    if (r < 0) {
        int read_err = errno;
        // errno still encodes the error from the failed read 
        close(fd1);
        close(fd2);
        // errno now encodes the status of the close(fd2) syscall
        
        switch(read_err) {
            case EISDIR: {
                errx(5, "File %s is a directory!", argv[1]);
            default:
                errx(6, "Failed to read from file %s", argv[2]);
            }
        }
    }

    close(fd2);
    close(fd1);
    exit(0);
}

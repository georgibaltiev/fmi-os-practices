#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>

int wrapped_open(const char* filename, int mode, mode_t* access) {
    
    int fd;

    if (access == NULL) {
        fd = open(filename, mode);
    } else {
        fd = open(filename, mode, *access);
    }

    if (fd < 0) {
        err(2, "failed to open %s", filename);
    }

    return fd;
}

int wrapped_read(int fd, void* buffer, size_t size) {

    int r = read(fd, buffer, size);

    if (r < 0) {
        err(3, "failed to read %d", fd);
    }

    return r;
}


int wrapped_write(int fd, const void* buffer, size_t size) {

    int w = write(fd, buffer, size);

    if (w < 0) {
        err(3, "failed to read %d", fd);
    }

    return w;
}

off_t get_file_size(int fd) {
    
    struct stat s;

    if (fstat(fd, &s) < 0) {
        err(1, "Failed to stat");
    }

    return s.st_size;

}

typedef struct {
    uint16_t offs;
    uint8_t one;
    uint8_t two;
} patch;

int main(int argc, char* argv[]) {

    if (argc != 4) {
        errx(1, "Failed args!");
    }

    int fd1 = wrapped_open(argv[1], O_RDONLY, NULL);
    int fd2 = wrapped_open(argv[2], O_RDONLY, NULL);
    
    off_t size1 = get_file_size(fd1);
    off_t size2 = get_file_size(fd2);

    if (size1 != size2) {
        errx(1, "Unable to patch %s and %s - they have different sizes", argv[1], argv[2]);
    }

    mode_t access = S_IRUSR | S_IWUSR;
    int pat = wrapped_open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, &access);

    uint8_t c1, c2;
    uint16_t offset = 0;

    while (wrapped_read(fd1,&c1, sizeof(c1)) > 0 && wrapped_read(fd2, &c2, sizeof(c2))) {
        
        if (c1 != c2) {
            patch p;
            p.offs = offset;
            p.one = c1;
            p.two = c2;
            wrapped_write(pat, &p, sizeof(p));
        }

        offset++;
    }

    close(pat);
    close(fd2);
    close(fd1);
    exit(0);
}

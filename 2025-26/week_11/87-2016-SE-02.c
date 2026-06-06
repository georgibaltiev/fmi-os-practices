#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdio.h>

typedef struct {
    uint32_t offset;
    uint32_t length;
} packet;

int wrapped_open(const char* filename, int mode, int* perm); 

int wrapped_open(const char* filename, int mode, int* perm) {
    
    int fd;

    if (perm) {
        fd = open(filename, mode, *perm);
    } else {
        fd = open(filename, mode);
    }
    
    if (fd < 0) {
        err(1, "Failed to open file %s", filename);
    }

    return fd;
}

int wrapped_read(int fd, void* buff, size_t buff_size);

int wrapped_read(int fd, void* buff, size_t buff_size) {
    
    int r = read(fd, buff, buff_size);
    if (r < 0) {
        err(2, "Failed to read %ld bytes from fd %d", buff_size, fd);
    }

    return r;
}

int wrapped_write(int fd, const void* buff, size_t buff_size); 

int wrapped_write(int fd, const void* buff, size_t buff_size) {

    int w = write(fd, buff, buff_size);
    if (w < 0) {
        err(3, "Failed to write %ld bytes from fd %d", buff_size, fd);
    }

    return w;
}

off_t wrapped_lseek(int fd, off_t offset, int whence); 

off_t wrapped_lseek(int fd, off_t offset, int whence) {

    off_t l = lseek(fd, offset, whence);
    if (l < 0) {
        err(4, "Failed to lseek at %ld in fd %d", l, fd);
    }

    return l;
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        errx(5, "Parameters!");
    }

    int f1_fd = wrapped_open(argv[1], O_RDONLY, NULL);
    
    // retain original position of lseek
    off_t offset_1 = wrapped_lseek(f1_fd, 0, SEEK_CUR); 
    
    // get size
    off_t f1_size = wrapped_lseek(f1_fd, 0, SEEK_END);

    // restore previous offset
    wrapped_lseek(f1_fd, offset_1, SEEK_SET);

    if (f1_size % sizeof(packet)) {
        errx(1, "File 1 is not the correct size");
    }

    int f2_fd = wrapped_open(argv[2], O_RDONLY, NULL);

    // retain original position of lseek
    off_t offset_2 = wrapped_lseek(f2_fd, 0, SEEK_CUR); 
    
    // get size
    off_t f2_size = wrapped_lseek(f2_fd, 0, SEEK_END);

    // restore previous offset
    wrapped_lseek(f2_fd, offset_2, SEEK_SET);

    if (f2_size % sizeof(uint32_t)) {
        errx(1, "File 2 is not the correct size!");
    }

    // validating logic
    packet p;
    while (wrapped_read(f1_fd, &p, sizeof(p)) > 0) {
        if (p.offset + p.length > (f2_size / sizeof(uint32_t))) {
            errx(1, "Incorrect offset + length");
        }
    }

    wrapped_lseek(f1_fd, 0, SEEK_SET);

    int perm = S_IRUSR | S_IWUSR;
    int f3_fd = wrapped_open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, &perm);

    // actual writing logic
    while (wrapped_read(f1_fd, &p, sizeof(p)) > 0) {
        
        wrapped_lseek(f2_fd, p.offset * sizeof(uint32_t), SEEK_SET);
        // we read and write <length> amount of nums
        uint32_t number;
        for (uint32_t i = 0; i < p.length; i++) {
            wrapped_read(f2_fd, &number, sizeof(number));
            wrapped_write(f3_fd, &number, sizeof(number));
        }
    }
    
    close(f3_fd);
    close(f2_fd);
    close(f1_fd);

}

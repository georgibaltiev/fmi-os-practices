#include <fcntl.h>
#include <err.h>
#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>

off_t asserted_lseek(int fd, off_t offset, int whence) {
    
    off_t ls = lseek(fd, offset, whence);

    if (ls < 0) {
        err(errno, "Failed to lseek in %d", fd);
    }

    return ls;
}

int asserted_write(int fd, const void* buffer, int size) {

    int w = write(fd, buffer, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }

    return w;

}

int asserted_read(int fd, void* buffer, int size) {
    
    int r = read(fd, buffer, size);

    if (r < 0) {
        err(1, "Failed to read from %d", fd);
    }

    return r;
}

int asserted_open(const char* filename, int mode, int* options) {
    
    int fd;

    if (options != NULL) {
        fd = open(filename, mode, *options);
    } else {
        fd = open(filename, mode);
    }
    
    if (fd < 0) {
        err(1, "Failed to open file %s", filename);
    }

    return fd;
}

typedef struct {
    uint16_t offset;
    uint8_t length;
    uint8_t magic; // padding 
} index;

int main(int argc, char* argv[]) {

    if (argc != 5) {
        errx(1, "Invalid amount of parameters! Expected 4 instead of %d", argc - 1);
    }
    
    int f1_dat = asserted_open(argv[1], O_RDONLY, NULL);
    int f1_idx = asserted_open(argv[2], O_RDONLY, NULL);

    off_t f1_dat_len = asserted_lseek(f1_dat, 0, SEEK_END);
    asserted_lseek(f1_dat, 0, SEEK_SET);

    off_t f1_idx_len = asserted_lseek(f1_idx, 0, SEEK_END);
    asserted_lseek(f1_idx, 0, SEEK_SET);

    if (f1_idx_len % sizeof(index)) {
        errx(1, "File %s does not have the correct size!", argv[2]);
    }

    index i1;

    while (asserted_read(f1_idx, (void*) &i1, sizeof(i1)) > 0) {
        
        if (i1.offset + i1.length > f1_dat_len) {
            errx(1, "Invalid offset in %s - exceeds length of %s", argv[2], argv[1]);
        }

    }

    int mode = S_IRUSR | S_IWUSR;

    int f2_dat = asserted_open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, &mode);
    int f2_idx = asserted_open(argv[4], O_WRONLY | O_CREAT | O_TRUNC, &mode);

    index i2;

    off_t current_offset = 0;
    char c;

    asserted_lseek(f1_idx, 0, SEEK_SET);

    while (asserted_read(f1_idx, (void*) &i1, sizeof(i1)) > 0) {
        
        asserted_lseek(f1_dat, i1.offset, SEEK_SET);
        asserted_read(f1_dat, &c, sizeof(char));

        asserted_write(1, (void*) &c, sizeof(c));

        if (c >= 'A' && c <= 'Z') {
            
            asserted_write(f2_dat, (const void*) &c, sizeof(c));

            for (int i = 1; i < i1.length; i++) {
                
                asserted_read(f1_dat, (void*) &c, sizeof(c));
                asserted_write(f2_dat, (const void*) &c, sizeof(c));
 
            }

            i2.offset = current_offset;
            i2.length = i1.length;
            i2.magic = 0x00;

            current_offset += i1.length;

            asserted_write(f2_idx, (const void*) &i2, sizeof(i2));

        }
    }

    close(f2_idx);
    close(f2_dat);

    close(f1_idx);
    close(f1_dat);

    exit(0);
}

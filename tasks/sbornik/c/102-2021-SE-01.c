#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>

int wrapped_write(int fd, const void* buff, size_t size) {

    int w = write(fd, buff, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }
    
    return w;
}

int wrapped_read(int fd, void* buff, size_t size) {
    
    int r = read(fd, buff, size);

    if (r < 0) {
        err(1, "Failed to read from %d", fd);
    }

    return r;
}

int wrapped_open(const char* filename, int mode, int* access) {

    int fd;

    if (access) {
        fd = open(filename, mode, *access);
    } else {
        fd = open(filename, mode);
    }
    
    return fd;
}

int main(int argc, char* argv[]) {
    
    if (argc != 3) {
        errx(1, "args");
    }

    int in_fd = wrapped_open(argv[1], O_RDONLY, NULL);
    
    int access = S_IRUSR | S_IWUSR;
    int out_fd = wrapped_open(argv[2], O_RDWR | O_CREAT | O_TRUNC, &access);
    
    uint8_t buff;
    int r;
    while ((r = wrapped_read(in_fd, &buff, sizeof(buff))) > 0) {
         
        uint8_t mask = 1;
        uint16_t encoded = 0x0000;
            
        for (int i = 0; i < 8; i++) {
            if (buff & (mask << i)) {
                encoded |= 2 << (2 * i);
            } else {
                encoded |= 1 << (2 * i);
            }
        }

        // трябва да внимаваме с endinaness-а на данните като ги записваме
        uint8_t* encoded_ptr = (uint8_t*) &encoded;

        wrapped_write(out_fd, encoded_ptr + 1, sizeof(uint8_t));
        wrapped_write(out_fd, encoded_ptr, sizeof(uint8_t)); 
    }

    close(out_fd);
    close(in_fd);
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

off_t wrapped_lseek(int fd, off_t offset, int whence) {

    off_t l = lseek(fd, offset, whence);

    if (l < 0) {
        err(1, "Failed to lseek");
    }

    return l;

}

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
        errx(1, "Invalid amount of parameters - expected 2 but got %d", argc - 1);
    }

    if (strlen(argv[1]) > 63) {
        errx(1, "The searched word is longer than 63 characters!");
    }

    int dic_fd = wrapped_open(argv[2], O_RDONLY, NULL);

    off_t begin = 0;
    off_t end = wrapped_lseek(dic_fd, 0, SEEK_END);
    
    while(begin < end) {
    
        off_t mid = begin + (end - begin) / 2;

        char buffer[64];
        
        wrapped_lseek(dic_fd, mid, SEEK_SET);
        
        char buff;
        int r;
        
        // обхождаме докато не намерим начален символ за дума
        while ((r = wrapped_read(dic_fd, &buff, sizeof(buff))) > 0) {
            if (buff == 0x00) {
                break;
            }
        }

        int idx = 0;
    
        // изчитаме думата
        while((r = wrapped_read(dic_fd, &buff, sizeof(buff))) > 0) {
            buffer[idx] = buff;
            idx++;

            if (buff == '\n') {
                break;
            }
        }
        
        buffer[idx - 1] = '\0';

        int cmp = strcmp(argv[1], buffer);
        
        if (cmp > 0) {
            begin = mid + 1;
        } else if (cmp < 0) {
            end = mid - 1;
        } else {
            while ((r = wrapped_read(dic_fd, &buff, sizeof(buff))) > 0 && buff != 0x00) {
                wrapped_write(1, &buff, sizeof(buff));
            }

            wrapped_write(1, "\n", strlen("\n"));
            close(dic_fd);
            exit(0);
        }

    }
    
    close(dic_fd);
    errx(1, "Can't find %s in the dictionary", argv[1]);
}

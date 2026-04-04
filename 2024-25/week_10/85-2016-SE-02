#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>

typedef struct {
    uint32_t offset;
    uint32_t length;
} pair;

// мои имплементации на wrapper функции - с тях си спестяваме ненужното повтаряне на проверки в main метода
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

off_t wrapped_lseek(int fd, off_t offset, int whence) {

    off_t l = lseek(fd, offset, whence);

    if (l < 0) {
        err(4, "Faild to lseek %ld", l);
    }

    return l;
}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        errx(1, "Invalid amount of params!");
    }

    int fd1 = wrapped_open(argv[1], O_RDONLY, NULL);
    
    off_t size1 = wrapped_lseek(fd1, 0, SEEK_END);

    // валидираме дали f1.bin е поредица от чифтове от 4-байтови числа
    if (size1 % 8) {
        errx(1, "Invalid file size - %s", argv[1]);
    }
    
    wrapped_lseek(fd1, 0, SEEK_SET);

    int fd2 = wrapped_open(argv[2], O_RDONLY, NULL);
    
    off_t size2 = wrapped_lseek(fd2, 0, SEEK_END);

    // аналогично за f2.bin
    if (size2 % 4) {
        errx(1, "Invalid file size - %s", argv[1]);
    }

    wrapped_lseek(fd2, 0, SEEK_SET);

    mode_t access = S_IRUSR | S_IWUSR;

    int fd3 = wrapped_open(argv[3], O_WRONLY | O_CREAT | O_TRUNC, &access);

    pair p;

    while(wrapped_read(fd1, &p, sizeof(p)) > 0) {

        // Тук има грешка - поправена е на 09/05/25 - изпуснато е умножението на offset-а по sizeof()
        if (((p.offset + p.length) * sizeof(uint32_t)) > size2) {
            errx(1, "Invalid offset of %s", argv[1]);
        }
    }

    wrapped_lseek(fd1, 0, SEEK_SET);

    while (wrapped_read(fd1, &p, sizeof(p)) > 0) {
        
        wrapped_lseek(fd2, p.offset * sizeof(uint32_t), SEEK_SET);
        
        uint32_t num;

        for (uint32_t i = 0; i < p.length; i++) {

            wrapped_read(fd2, &num, sizeof(num));
            wrapped_write(fd3, &num, sizeof(num));

        }

    }

    close(fd3);
    close(fd2);
    close(fd1);
    exit(0);
}

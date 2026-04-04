/*
	Реализирайте команда swap, разменяща съдържанието на два файла, подадени като
	входни параметри. Приемаме, че двата файла имат еднакъв брой символи. Може да
	модифицирате решението, да работи и когато нямат еднакъв брой символи.
*/

#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>

// вариант 1 - работим с 2 файла с еднаква дължина

off_t asserted_lseek(int fd, off_t offset, int whence) {
    
    off_t ls = lseek(fd, offset, whence);

    if (ls < 0) {
        err(3, "Failed to lseek in %d", fd);
    }

    return ls;
}

int main(int argc, char* argv[]) {
	
    if (argc < 3) {
        errx(4, "Amount of params!");
    }

    int fd1 = open(argv[1], O_RDWR);

    if (fd1 < 0) {
        err(1, "Failed to open %s", argv[1]);
    }
    
    int fd2 = open(argv[2], O_RDWR);

    if (fd2 < 0) {
        err(1, "Failed to open %s", argv[2]);
    }

    char c1;
    char c2;

    int r1;
    int r2;

    while (((r1 = read(fd1, &c1, sizeof(c1))) > 0) && ((r2 = read(fd2, &c2, sizeof(c2))) > 0)) {
         
        asserted_lseek(fd1, -1, SEEK_CUR);
        asserted_lseek(fd2, -1, SEEK_CUR);

        if (write(fd1, &c2, sizeof(c2)) < 0) {
            err(2, "failed to write to %s", argv[1]);
        }

        if (write(fd2, &c1, sizeof(c1)) < 0) {
            err(2, "failed to write to %s", argv[2]);
        }
    }

    if (r1 < 0) {
        err(1, "Failed to write to %s", argv[1]);
    }
    
    if (r2 < 0) {
        err(1, "Failed to write to %s", argv[2]);
    }

    close(fd1);
    close(fd2);
    exit(0);

}

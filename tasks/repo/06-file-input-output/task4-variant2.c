/*
	Реализирайте команда swap, разменяща съдържанието на два файла, подадени като
	входни параметри. Приемаме, че двата файла имат еднакъв брой символи. Може да
	модифицирате решението, да работи и когато нямат еднакъв брой символи.
*/

#include <unistd.h>
#include <fcntl.h>
#include <err.h>
#include <stdlib.h>

// втори вариант - ако дължините на двата файла се различават, можем да използваме временен буфер
int main(int argc, char* argv[]) {
	
    if (argc < 3) {
        errx(4, "Amount of params!");
    }

    int fd1 = open(argv[1], O_RDONLY);

    if (fd1 < 0) {
        err(1, "Failed to open %s", argv[1]);
    }
    
    // приемаме, че съдържанието на първия файл може да се запълни в 4096 байта за улеснение на задачата - това условие е доста лесно да бъде счупено
    char buffer[4096];

    int read_size = read(fd1, buffer, sizeof(buffer));

    if (read_size < 0) {
        err(1, "Failed to read from %s", argv[1]);
    }

    close(fd1);

    int fd2 = open(argv[2], O_RDONLY);

    if (fd2 < 0) {
        err(1, "Failed to open %s", argv[2]);
    }

    // наново отваряме първия файл, но само за писане

    fd1 = open(argv[1], O_WRONLY | O_TRUNC);

    if (fd1 < 0) {
        err(1, "Failed to open %s", argv[1]);
    }

    char buff;
    int r;

    while ((r = read(fd2, &buff, sizeof(buff))) > 0) {
        
        int w = write(fd1, &buff, sizeof(buff));

        if (w < 0) {
            err(1, "Failed to write to %s", argv[1]);
        }
    }

    if (r < 0) {
        err(1, "Failed to read from %s", argv[2]);
    }

    close(fd1);
    close(fd2);

    // наново отваряме fd2, но само за писане

    fd2 = open(argv[2], O_WRONLY | O_TRUNC);    

    int w = write(fd2, buffer, read_size);

    if (w < 0) {
        err(1, "Failed to write to %s", argv[2]);
    }

    if (w != read_size) {
        errx(1, "Didn't write enough bytes to %s", argv[2]);
    }

    close(fd2);
    exit(0);

}

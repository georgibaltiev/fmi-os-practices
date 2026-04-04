/*
	Реализирайте команда head без опции (т.е. винаги да извежда
	на стандартния изход само първите 10 реда от съдържанието на
	файл подаден като първи параматър)
*/

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>

int main(int argc, char* argv[]) {
	
    if (argc != 2) {
        errx(1, "Invalid amount of params!");
    }

    int fd = open(argv[1], O_RDONLY);
    
    if (fd < 0) {
        err(2, "Failed to open %s", argv[1]);
    }

    char buf;
    int r;

    int line = 0;

    while ((r = read(fd, &buf, sizeof(buf))) > 0) {
            
        if (buf == '\n') {
            line++;
        }

        if (write(1, &buf, sizeof(buf)) < 0) {
            err(4, "Failed to write");
        }

        if (line == 10) {
            break;
        }

    }

    close(fd);
    exit(0);
}

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>

//1 - копирайте съдържанието на файл1 във файл2
// ./main <file1> <file2> 
int main(int argc, char* argv[]) {

    if (argc != 3) {
        errx(1, "Amount of params!");
    }

    int fd1 = open(argv[1], O_RDONLY);
    
    if (fd1 < 0) {
        err(2, "%s failed to open", argv[1]);
    }

    int fd2 = open(argv[2], O_WRONLY | O_TRUNC);


    if (fd2 < 0) {
        
        // ако искаме да използваме errno номера, трябва да внимаваме в кой момент запазваме неговата стойност
        // например

        // int error = errno;
        
        // errno съдържа грешката на open
        close(fd1);
        // errno съдържа грешката от close
        
        // от тук примерно можем да обработим errno стойността
        /*switch(error) {
            case ENOENT: ...; break;
            case EACCES: ...; break; 
        }*/
        
        err(2, "%s failed to open", argv[2]);
    }

    char buff[4096];

    int r;
    int w;

    while ((r = read(fd1, buff, sizeof(buff))) > 0) {
        
        w = write(fd2, buff, r);

        if (w < 0) {
            close(fd2);
            close(fd1);
            err(4, "Failed to write to %s", argv[2]);
        }

    }

    if (r < 0) {
        close(fd2);
        close(fd1);
        err(3, "Failed to read from %s", argv[1]);
    }

    close(fd2);
    close(fd1);
    exit(0);
}

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <string.h>

// Не използваме wrapper-и за да може да зачистим ресурсите при shutdown на програмата
// Имайте предвид, че това решение не е особено коректно, тъй като errno не е handle-нато по правилен начин, т.е. ако някое системно извикване се провали, ще изкара Success, ако unlink се изпълни успешно
// За да извжда коректни съобщения програмата, е нужно първо да си запазим errno променливата от проваленото системно извикване, и след това да направим unlink. После обаче трябва да се прави switch-case
// За да спестя boilerplate тук съм игнорирал това съображение
void wrapped_unlink(const char* path) {
    if (unlink(path) < 0) {
        err(1, "Failed to unlink file %s", (const char*) path);
    }
}

off_t get_file_size(int fd) {

    struct stat st;

    if (fstat(fd, &st) < 0) {
        err(2, "Failed to stat from fd %d", fd);
    }

    return st.st_size;

}

typedef struct {
    uint64_t next;
    char data[504];
} Node;

int main(int argc, char* argv[]) {

    if (argc != 2) {
        errx(3, "Invalid amount of parameters!");
    }

    char temp_path[] = "/tmp/jory_tempXXXXXXX";

    int file_fd = open(argv[1], O_RDWR);
    
    if (file_fd < 0) {
        err(4, "Failed to open %s", argv[1]);
    }
    
    off_t file_size = get_file_size(file_fd);

    if (file_size % sizeof(Node)) {
        errx(5, "%s is not in the correct size", argv[1]);
    }
    
    int temp_fd = mkstemp(temp_path);

    if (temp_fd < 0) {
        err(6, "Failed to open a temporary file");
    }
        
    char empty[sizeof(Node)];
    
    // или memset, но бих заложил на този цикъл
    for (long unsigned int i = 0; i < sizeof(Node); i++) {
        empty[i] = 0;
    }

    int all_nodes = file_size / sizeof(Node);

    for (int i = 0; i < all_nodes; i++) {
        if (write(temp_fd, &empty, sizeof(empty)) < 0) {
            wrapped_unlink(temp_path);
            err(7, "Failed to write to temp file!");
        }
    }

    if(lseek(temp_fd, 0, SEEK_SET) < 0) {
        wrapped_unlink(temp_path);
        err(8, "Failed to lseek in temo file!");
    }

    Node n;

    n.next = 0;

    do {
    
        int r = read(file_fd, &n, sizeof(n));
        
        if (r != sizeof(Node)) {
            wrapped_unlink(temp_path);        
            errx(9, "Error reading node from %s", argv[1]);
        }

        if (write(temp_fd, &n, sizeof(n)) < 0) {
            wrapped_unlink(temp_path);
            errx(10, "Failed to write to temp file!");
        }

        if (lseek(file_fd, n.next * sizeof(Node), SEEK_SET) < 0) {
            wrapped_unlink(temp_path);
            errx(11, "Failed to lseek in temp file!");
        }

        if (lseek(temp_fd, n.next * sizeof(Node), SEEK_SET) < 0) {
            wrapped_unlink(temp_path);
            errx(12, "Failed to lseek in temp file!");
        }

    } while (n.next);

    close(file_fd);

    if (lseek(temp_fd, 0, SEEK_SET) < 0) {
        wrapped_unlink(temp_path);
        errx(13, "Failed to lseek in temp file!");
    }
    
    file_fd = open(argv[1], O_WRONLY | O_TRUNC, NULL, temp_path);

    if (file_fd < 0) {
        wrapped_unlink(temp_path);
        errx(14, "Failed to open file %s", argv[1]);
    }

    int r;

    while((r = read(temp_fd, &n, sizeof(n))) > 0) {
        if (write(file_fd, &n, sizeof(n)) < 0) {
            wrapped_unlink(temp_path);
            errx(15, "Failed to write to %s", argv[1]);
        }
    }

    wrapped_unlink(temp_path);
    close(file_fd);
    exit(0);
}

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <sys/stat.h>
#include <string.h>

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

int from, to;
int col_idx = 1;
char newline = '\n';

// тук можем да си позволим да използваме аритметика със символи, вместо да конвертираме const char* буфер към int
// в други задачи, където ще се налага да се конвертират стрингове към числови стойности, ще използваме strtol (man 3 strtol)
int retrieve_num(char n) {
    
    if (n < '0' || n > '9') {
        errx(1, "args");
    }

    return n - '0';
}

void get_nums(const char* param) {
    
    if (strlen(param) != 1 && strlen(param) != 3) {
        errx(1, "Params!");
    }

    if (strlen(param) == 1) {
        from = retrieve_num(param[0]);
        to = from;
    } else {

        if (param[1] != '-') {
            errx(1, "The range parameter should be separated with a -: invalid %s", param);
        }

        from = retrieve_num(param[0]);
        to = retrieve_num(param[2]);
    
        if (from >= to) {
            errx(1, "Invalid interval - %d should be less than %d", from, to);
        }

    }
}

int main(int argc, char* argv[]) {

    if (argc == 3) {

        // случая, когато искаме да принтираме само конкретни символи на даден ред
        
        if (strcmp(argv[1], "-c")) {
            errx(1, "first flag should be -c!");
        }

        get_nums(argv[2]);

        char buf;

        while(wrapped_read(0, &buf, sizeof(buf)) > 0) {
            
            if (col_idx >= from && col_idx <= to) {
                wrapped_write(1, &buf, sizeof(buf));
            }

            col_idx++;

            if ('\n' == buf) {
                wrapped_write(1, &newline, sizeof(newline));
                col_idx = 1;
            }
        }

    } else if (argc == 5) {
    
        // случая, когато искаме да принтираме колони разделени с delimiter

        if (strcmp(argv[1], "-d")) {
            errx(1, "first flag should be -d!");
        }

        char delim = argv[2][0];

        if (strcmp(argv[3], "-f")) {
            errx(1, "the -d flag should be paired with -f!");
        }

        get_nums(argv[4]);

        char buf;

        while(wrapped_read(0, &buf, sizeof(buf)) > 0) {
            
            if (col_idx >= from && col_idx < to) {
                wrapped_write(1, &buf, sizeof(buf));
            }

            if (col_idx == to && buf != delim) {
                wrapped_write(1, &buf, sizeof(buf));
            }

            if (buf == delim) {
                col_idx++;
            }

            if (buf == '\n') {
                col_idx = 1;
                wrapped_write(1, &newline, sizeof(newline));
            }
    
        }
    
    } else {
        errx(1, "Invalid amount of parameters - expected 2 or 4 instead of %d!", argc - 1);
    }

    exit(0);
}

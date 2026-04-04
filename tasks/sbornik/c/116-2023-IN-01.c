#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>
#include <string.h>

pid_t wrapped_fork(void) {

    pid_t child = fork();
    
    if (child < 0) {
        err(1, "Failed to fork a child");
    }

    return child;
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

// Принципно този wrapper не бих препоръчал да се използва, но за тази задача върши достатъчно добра работа
int wrapped_wait(void) {

    int w_status;
    
    if (wait(&w_status) < 0) {
        err(1, "Failed to wait for child to finish");
    }

    if (!WIFEXITED(w_status)) {
        err(1, "Child exited abnormally!");
    }
    
    return w_status;
}

int wrapped_pipe(int pipefd[2]) {

    int p = pipe(pipefd);

    if (p < 0) {
        err(1, "Failed to pipe");
    }

    return p;

}

int pipes[8][2];
int nc;
int wc;

const char* words[] = {"tic ", "tac ", "toe\n"};

int main(int argc, char* argv[]) {


    if (argc != 3) {
        errx(1, "Invalid amount of parameters!");
    }

    nc = atoi(argv[1]);
    wc = atoi(argv[2]);

    if (nc < 1 || nc > 7) {
        errx(1, "Number of children should be in the range [1;7]");
    }

    if (wc < 1 || wc > 35) {
        errx(1, "Number of words should be in the range [1;35]");
    }

    for (int i = 0; i <= nc; i++) {
        wrapped_pipe(pipes[i]);
    }
    
    //Инициализираме, само за да се компилира
    int reading_end = 0;
    int writing_end = 0;

    for (int i = 1; i <= nc; i++) {
    
        pid_t child = wrapped_fork();

        if (child == 0) {
            
            for (int j = 0; j <= nc; j++) {
                
                if (j == i) {
                    close(pipes[j][0]);
                    writing_end = pipes[j][1];
                } else if (j == i - 1) {
                    close(pipes[j][1]);
                    reading_end = pipes[j][0];
                } else {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

            }

            int r;
            uint8_t index;

            while ((r = wrapped_read(reading_end, &index, sizeof(index))) > 0) {
                
                if (index > wc) {
                    wrapped_write(writing_end, &index, sizeof(index));
                    break;
                } else {
                    wrapped_write(1, words[index % 3], strlen(words[index % 3]));
                    index++;
                    wrapped_write(writing_end, &index, sizeof(index));
                }

            }

            close(reading_end);
            close(writing_end);
            exit(0);
        }

    }

    close(pipes[0][0]);
    close(pipes[nc][1]);

    for (int i = 1; i < nc; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
    
    reading_end = pipes[nc][0];
    writing_end = pipes[0][1];

    uint8_t index = 1;
    wrapped_write(1, words[0], strlen(words[0]));
    wrapped_write(writing_end, &index, sizeof(index));

    int r;
            
    while ((r = wrapped_read(reading_end, &index, sizeof(index))) > 0) {
                
        if (index > wc) {
            wrapped_write(writing_end, &index, sizeof(index));
            break;
        } else {
            wrapped_write(1, words[index % 3], strlen(words[index % 3]));
            index++;
            wrapped_write(writing_end, &index, sizeof(index));
        }
    
    }

    close(reading_end);
    close(writing_end);
    
    for (int i = 1; i <= nc; i++) {
        wrapped_wait();
    }


    exit(0);
}


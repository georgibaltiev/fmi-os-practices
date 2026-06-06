#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int wrapped_open(const char* filename, int mode, int* perm) {
    
    int fd;

    if (perm) {
        fd = open(filename, mode, *perm);
    } else {
        fd = open(filename, mode);
    }
    
    if (fd < 0) {
        err(1, "Failed to open file %s", filename);
    }

    return fd;
}

int wrapped_read(int fd, void* buff, size_t buff_size) {
    
    int r = read(fd, buff, buff_size);
    if (r < 0) {
        err(2, "Failed to read %ld bytes from fd %d", buff_size, fd);
    }

    return r;
}

int wrapped_write(int fd, const void* buff, size_t buff_size) {

    int w = write(fd, buff, buff_size);
    if (w < 0) {
        err(3, "Failed to write %ld bytes from fd %d", buff_size, fd);
    }

    return w;
}

off_t wrapped_lseek(int fd, off_t offset, int whence) {

    off_t l = lseek(fd, offset, whence);
    if (l < 0) {
        err(4, "Failed to lseek at %ld in fd %d", l, fd);
    }

    return l;
}

pid_t wrapped_fork(void) {

    pid_t child = fork();
    if (child < 0) {
        err(5, "Failed to fork");
    }

    return child;
}

int wrapped_pipe(int pipefd[2]) {

    int pfd = pipe(pipefd);
    if (pfd < 0) {
        err(7, "Failed to pipe");
    }

    return pfd;
}

int wrapped_dup2(int oldfd, int newfd) {

    int d = dup2(oldfd, newfd);
    if (d < 0) {
        err(8, "Failed to dup2");
    }
    
    return d;
}

int wrapped_wait(int* w_status) {

    int w = wait(w_status);
    if (w < 0) {
        err(9, "Failed to wait");
    }

    return w;
}

int main(void) {

    int driver_pipe[2];
    wrapped_pipe(driver_pipe);
    
    pid_t driver_p = wrapped_fork();
    if (driver_p == 0) {
        close(driver_pipe[0]);
        wrapped_dup2(driver_pipe[1], 1);
        close(driver_pipe[1]);
        
        execlp("./fake_driver", "./fake_driver", NULL);
        err(1, "Failed to exec driver");
    }

    close(driver_pipe[1]);

    int wheel_to_p[4][2];
    int p_to_wheel[4][2];

    for (int i = 0; i < 4; i++) {
        wrapped_pipe(wheel_to_p[i]);
        wrapped_pipe(p_to_wheel[i]);
    }



    for (int i = 0; i < 4; i++) {
        
        pid_t wheel_p = wrapped_fork();
        if (wheel_p == 0) {
            
            close(driver_pipe[0]);
    
            for (int j = 0; j < 4; j++) {
                
                if (j != i) {
                    close(wheel_to_p[j][0]);    
                    close(wheel_to_p[j][1]);
                    close(p_to_wheel[j][0]);
                    close(p_to_wheel[j][1]);
                } else {
                    close(wheel_to_p[i][0]);
                    close(p_to_wheel[i][1]);
                }
                
                wrapped_dup2(wheel_to_p[i][1], 1);
                wrapped_dup2(p_to_wheel[i][0], 0);
                
                close(wheel_to_p[i][1]);
                close(p_to_wheel[i][0]);

                execlp("./fake_wheel", "./fake_wheel", NULL);
                err(3, "Failed to exec wheel");
            }

        }

    }
        // parent
        for (int i = 0; i < 4; i++) {
            close(wheel_to_p[i][1]);
            close(p_to_wheel[i][0]);
        }

        uint16_t I = 0;

        while (1) {
            
            uint16_t gaz[8];
            wrapped_read(driver_pipe[0], gaz, sizeof(gaz));
                
            uint16_t omega = gaz[4];

            uint16_t speed[4] = {0};
            for (int i = 0; i < 4; i++) {
                uint16_t skorost[8];
                wrapped_read(wheel_to_p[i][0], skorost, sizeof(skorost));
                speed[i] = skorost[1];
            }

            uint16_t avg_speed = 0.00;
            for (int i = 0; i < 4; i++) {
                avg_speed += speed[i];
            }

            
            avg_speed /= 4;

            if (avg_speed < omega) {
                I++;
            } else {
                I--;
            }

            uint16_t tok[8] = {0};
            // I = tok[1] -> ОРИГИНАЛ
	    // КОРЕКЦИЯ
	    tok[1] = I;
       
            warnx("Gaz - %d, Skorost - %d, Tok - %d", omega, avg_speed, I);

            for (int i = 0; i < 4; i++) {
                wrapped_write(p_to_wheel[i][1], tok, sizeof(tok));
            }
        }

}

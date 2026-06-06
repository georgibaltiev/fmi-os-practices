
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

int wrapped_write(int fd, const void *ptr, size_t size) {

    int w = write(fd, ptr, size);

    if (w < 0) {
        err(1, "failed to write to %d", fd);
    }

    return w;
}

int wrapped_read(int fd, void *ptr, size_t size) {

    int r = read(fd, ptr, size);

    if (r < 0) {
        err(2, "failed to read from %d", fd);
    }

    return r;
}

int wrapped_pipe(int pipefd[2]) {

    int p = pipe(pipefd);
    if (p < 0) {
        err(3, "failed to pipe");
    }

    return p;
}

int wrapped_dup2(int oldfd, int newfd) {

    int d = dup2(oldfd, newfd);
    if (d < 0) {
        err(4, "failed to duplicate %d to %d", oldfd, newfd);
    }

    return d;
}

pid_t wrapped_fork(void) {

    pid_t pid = fork();

    if (pid < 0) {
        err(5, "failed to fork");
    }

    return pid;
}

int main(void) {

    // тръба за получаване на данни от процеса-шофьор
    int from_driver[2];
    wrapped_pipe(from_driver);

    // 4 тръби за изпращане на данни към процесите-колела
    int main_to_wheels[4][2];
    for (int i = 0; i < 4; i++) {
        wrapped_pipe(main_to_wheels[i]);
    }

    // 4 тръби за получаване на данни от процесите-колела
    int wheels_to_main[4][2];
    for (int i = 0; i < 4; i++) {
        wrapped_pipe(wheels_to_main[i]);
    }

    // създаваме процеса-шофьор
    pid_t child = wrapped_fork();
    if (child == 0) {
        wrapped_dup2(from_driver[1], 1);
        close(from_driver[1]);
        close(from_driver[0]);

        for (int i = 0; i < 4; i++) {
            close(main_to_wheels[i][1]);
            close(main_to_wheels[i][0]);
        }

        for (int i = 0; i < 4; i++) {
            close(wheels_to_main[i][1]);
            close(wheels_to_main[i][0]);
        }

        execlp("./sample_data/fake_driver", "./sample_data/fake_driver", NULL);
        err(5, "failed to exec fake driver process");
    }

    close(from_driver[1]);

    // създаваме процесите-деца
    for (int i = 0; i < 4; i++) {
        child = wrapped_fork();
        if (child == 0) {
            close(from_driver[0]);

            wrapped_dup2(main_to_wheels[i][0], 0);
            close(main_to_wheels[i][0]);
            close(main_to_wheels[i][1]);

            wrapped_dup2(wheels_to_main[i][1], 1);
            close(wheels_to_main[i][1]);
            close(wheels_to_main[i][0]);

            // затваряме ненужните краища
            for (int j = 0; j < 4; j++) {
                
                if (j != i) {
                    close(main_to_wheels[j][0]);
                    close(main_to_wheels[j][1]);
                    close(wheels_to_main[j][0]);
                    close(wheels_to_main[j][1]);
                }
            }

            execlp("./sample_data/fake_wheel", "./sample_data/fake_wheel", NULL);
            err(6, "failed to exec fake wheel process");
        }
    }

    for (int i = 0; i < 4; i++) {
        close(main_to_wheels[i][0]);
        close(wheels_to_main[i][1]);
    }

    uint16_t I = 0;

    while (1) {
        uint16_t driver_package[8];
        wrapped_read(from_driver[0], driver_package, sizeof(driver_package));

        uint16_t gas = driver_package[4];
        uint16_t average_speed = 0;

        for (int i = 0; i < 4; i++) {
            uint16_t speed_package[8];
            wrapped_read(wheels_to_main[i][0], speed_package, sizeof(speed_package));
            uint16_t speed = speed_package[1];
            average_speed += speed;
        }
        average_speed /= 4;

        if (average_speed < gas) {
            I++;
        } else {
            I--;
        }

        uint16_t electric_package[8];
        memset(electric_package, 0, sizeof(electric_package));
        electric_package[1] = I;

        // за дебъгване
        warnx("gas: %d, avg speed: %d, I: %d", gas, average_speed, I);

        for (int i = 0; i < 4; i++) {
            wrapped_write(main_to_wheels[i][1], electric_package, sizeof(electric_package));
        }
    }
}

#include <unistd.h>
#include <fcntl.h>
#include <stdint.h>
#include <err.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

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

pid_t wrapped_fork(void) {

    pid_t child = fork();
    if (child < 0) {
        err(5, "Failed to fork");
    }

    return child;
}

int wrapped_dup2(int oldfd, int newfd) {

    int d = dup2(oldfd, newfd);
    if (d < 0) {
        err(8, "Failed to dup2");
    }
    
    return d;
}
 
pid_t pids[10];

pid_t start_child(const char* executable) {
    
    pid_t child = wrapped_fork();
    // if (child < 0) { -> ОРИГИНАЛЕН РЕД
    // ^ Грешката е била във факта, че съм написал грешно проверката дали сме в тялото на процеса-дете
    if (child == 0) { // -> КОРЕКЦИЯ
    
        // ДОПЪЛНЕНИЕ: в условието се изисква да не принтираме нищо, което програмата би изкарала на STDOUT или STDERR
        int null_fd = wrapped_open("/dev/null", O_WRONLY, NULL);
        wrapped_dup2(null_fd, 1);
        wrapped_dup2(null_fd, 2);
        close(null_fd);

        execlp(executable, executable, NULL);
        err(1, "failed to exec %s", executable);
    }

        return child;
}

int find_child(pid_t child) {

    for (int i = 0; i < 10; i++) {
        if (pids[i] == child) {
            return i;
        }
    }

    return -1;
}

int main(int argc, char* argv[]) {
    
    if (argc < 1 || argc > 11) {
        errx(1, "Params!");
    }

    // by default is 0 if no problems occur
    int exit_status = 0;

    // initial start of the processes
    for (int i = 1; i < argc; i++) {
        pids[i - 1] = start_child(argv[i]);
    }

    pid_t waited_child;
    int w_status;
  
    while((waited_child = wait(&w_status)) > 0) {

        if (WIFSIGNALED(w_status)) {

            // warnx("SIGNALED"); -> Разкарвам ненужни принтове

            int index = find_child(waited_child);
            pids[index] = 0;
            exit_status = index + 1;
            break;
        } else if (WEXITSTATUS(w_status) == 0) {
          
            // warnx("IM HERE"); -> Разкарвам ненужни принтове
            int index = find_child(waited_child);
            
            if (index < 0) {
                errx(1, "Process with pid %d not found!", waited_child); 
            }

            pids[index] = 0;
        } else {
            int index = find_child(waited_child);
            pids[index] = start_child(argv[index + 1]);
        }
        
   }

    // ДОПЪЛНЕНИЕ: все пак трябва да проверя дали while цикъла е приключил, заради failure нa wait-а, тоест добавям проверката за waited_child
    // if (errno != ECHILD) { -> оригинал
    if (waited_child < 0 && errno != ECHILD) {
        err(1, "Failed to wait");
    }

    for (int i = 0; i < 10; i++) {
        if (pids[i] > 0) {
            // ДОПЪЛНЕНИЕ: не е изключено да се опитаме да изпратим сигнал на процес, който вече да е приключил изпълнението си,
            // тъй като не сме ъпдейтвали масива от PID-овете, след като сме излезнали от while цикъла
            // Игнорираме ESRCH (man 2 kill) грешките, тъй като те сигнализират, че процеса не съществува, което е допустимо за рамките на решението.
            if (kill(pids[i], SIGTERM) < 0 && errno != ESRCH) {
                err(14, "Failed to kill %d", pids[i]);
            }
        }
    }

    for (int i = 0; i < 10; i++) {
        if (pids[i] > 0) {
            if (wait(&w_status) < 0) {
                err(15, "Failed to wait");
            }
        }
    }

    exit(exit_status);
}

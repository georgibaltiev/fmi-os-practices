#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <err.h>
#include <errno.h>
#include <signal.h>

pid_t wrapped_fork(void) {

    pid_t child = fork();
    
    if (child < 0) {
        err(1, "Failed to fork a child");
    }

    return child;
}

int wrapped_open(const char* filename, int mode, int* access) {

    int fd;

    if (access) {
        fd = open(filename, mode, *access);
    } else {
        fd = open(filename, mode);
    }
    
    return fd;
}

int wrapped_dup2(int oldfd, int newfd) {
    
    int d = dup2(oldfd, newfd);

    if (d < 0) {
        err(1, "Failed to duplicate %d to %d", oldfd, newfd);
    }

    return d;
}

int null_fd;

// Запазваме си един глобален масив, в който да пазим PID-а на конкретния процес, който в момента изпълнява програмата, на която е индексиран
pid_t runner_pids[10] = {0};

pid_t start_child(char* pathname) {

    pid_t child = wrapped_fork();

    if (child == 0) {
        
        dup2(null_fd, 1);
        dup2(null_fd, 2);
        execlp(pathname, pathname, (const char*) NULL);
        err(1, "Failed to exec %s", pathname);
    }

    return child;
}

int get_pid_idx(int pid) {

    for (int i = 0; i < 10; i++) {
        if (pid == runner_pids[i]) {
            return i;
        }
    }

    return -1;
}


int main(int argc, char* argv[]) {

    if (argc < 1 || argc > 11) {
        errx(1, "args");
    }

    int exit_status = 0;

    null_fd = wrapped_open("/dev/null", O_WRONLY, NULL);

    // Първо стартираме всички процеси по веднъж
    for (int i = 1; i < argc; i++) {
        
        pid_t child = start_child(argv[i]);
        runner_pids[i - 1] = child;
    
    }

    int w;
    int w_status;

    // Въртим while цикъл, докато вече нямаме процеси, които да изчакваме - това означава че или всички са завършили, или е настъпила друга грешка
    while((w = wait(&w_status)) > 0) {
    
        // Проверяваме на коя програма съответства изчакания процес
        int pid_idx = get_pid_idx(w);

        if (pid_idx < 0) {
            errx(1, "Process with id %d not found in table!", w);
        }
        
        // Ако процеса е завършил успешно, не го стартираме наново
        if (WIFEXITED(w_status) && WEXITSTATUS(w_status) == 0) {
            runner_pids[pid_idx] = 0;
        
        // Ако процеса е убит от сигнал, прекъсваме цикъла
        } else if (WIFSIGNALED(w_status)) {
            runner_pids[pid_idx] = 0;
            exit_status = pid_idx + 1;
            break;

        // Ако процеса е крашнал поради друга причина, го стартираме наново
        } else {
            runner_pids[pid_idx] = start_child(argv[pid_idx + 1]);
        }

    }

    // В случая, в който wait извикването ни се провали, но причината е поради друг проблем, а не защото повече деца няма за изчакване, fail-ваме
    if (w < 0 && errno != ECHILD) {
        err(2, "Failed to wait for child");
    }
    
    close(null_fd);

    for (int i = 0; i < argc - 1; i++) {
        
        // За всяко едно от оставащите активни деца (тоест тези, които не са занулени от горния цикъл), се опитваме да ги убием
        if (runner_pids[i] > 0) {
            
            // Ако не можем да убием конкретния процес, е възможно да е поради факта, че е приключил изпълнението си преди да се доберем до него
            // Поради това, игнорираме този случай (тъй като errno ще е равно на ESRCH)
            if (kill(runner_pids[i], SIGTERM) < 0) {
                if (errno != ESRCH) {
                    err(1, "Failed to kill pid %d", runner_pids[i]);
                }
            }
        }

    }

    for (int i = 0; i < argc - 1; i++) {
        
        // Отчитаме всички процеси, които не са занулени от първия цикъл
        if (runner_pids[i] > 0) {
            
            if (wait(&w_status) < 0) {
                err(1, "Failed to wait child");
            }
        }

    }

    exit(exit_status);
}

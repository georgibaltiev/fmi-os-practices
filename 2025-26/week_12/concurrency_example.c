#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char* parent = "parent\n";
const char* child = "child\n";

int main(void) {
    
    pid_t p = fork();

    if (p == 0) {
        for (int i = 0; i < 100; i++) {
            if (write(1, child, strlen(child)) < 0) {
                err(1, "Failed to write in child");
            }
        }
        exit(0);
    }
    
        for (int i = 0; i < 100; i++) {
            if (write(1, parent, strlen(parent)) < 0) {
                err(1, "Failed to write in parent");
            }
        }

    exit(0);
}

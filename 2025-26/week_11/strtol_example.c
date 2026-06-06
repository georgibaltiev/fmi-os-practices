#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdint.h>

int main(void) {

    uint8_t a = 0xf6;
    char buffer[10] = {0};

    snprintf(buffer, sizeof(buffer), "%d", a);

    write(1, buffer, sizeof(buffer));
}

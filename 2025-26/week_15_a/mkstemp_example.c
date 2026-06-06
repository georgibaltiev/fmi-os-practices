#include <stdlib.h>
#include <string.h>
#include <err.h>

int main(void) {


    char array[100] = {0};
    strcat(array, "/tmp/XXXXXX");

    int m = mkstemp(array);
    if (m < 0) {
        err(1, "Failed to create temp file");
    }

    warnx("%s", array);

}

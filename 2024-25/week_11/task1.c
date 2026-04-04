#include <unistd.h>
#include <err.h>

int main(void) {
    execlp("date", "date", (const char*) NULL); 
    err(1, "failed to execute date");
}

#include <unistd.h>
#include <err.h>

int main(void) {
	
    execl("/usr/bin/sleep", "/usr/bin/sleep", "4", (const char*) NULL);
    err(1, "Failed to execute sleep!");
}

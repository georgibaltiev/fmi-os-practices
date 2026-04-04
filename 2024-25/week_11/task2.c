#include <unistd.h>
#include <err.h>

int main(int argc, char* argv[]) {
	
    if (argc != 2) {
        errx(1, "Invalid args, expected 1 param");
    }

    execlp("ls", "ls", argv[1], (const char*) NULL);
    err(1, "Failed to execute ls command!");
}

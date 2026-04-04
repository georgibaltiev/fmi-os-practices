#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdlib.h>
#include <err.h>
#include <string.h>

int input_fds[20];
uint64_t identifiers[20];
uint8_t lengths[20];
char names[20][128];
char cue[128];

int wrapped_write(int fd, const void* ptr, int len) {

	int w = write(fd, ptr, len);
	
	if (w < 0) {
		err(1, "Failed to write to fd %d", fd);
	}

	return w;
}

int wrapped_read(int fd, void* ptr, int len) {

	int r = read(fd, ptr, len);

	if (r < 0) {
		err(1, "Failed to read from fd %d", fd);
	}

	return r;
}

int wrapped_open(const char* filename, int mode, int* perm) {

	int fd;

	if (perm) {
		fd = open(filename, mode, perm); 
	} else {
		fd = open(filename, mode);
	}

	if (fd < 0) {
		err(1, "Failed to open file");
	}

	return fd;
}

off_t get_file_size(int fd) {
	
	struct stat st;

	if (fstat(fd, &st) < 0) {
		err(1, "Failed to stat fd %d", fd);	
	}

	return st.st_size;
}

// прочитаме първия елемент от файла, проверяваме магическата стойност и запазваме името
void validate_file(int fd, int index) {
	
	wrapped_read(fd, identifiers + index, sizeof(uint64_t));
	wrapped_read(fd, lengths + index, sizeof(uint8_t));

	if (identifiers[index] != 133742) {
		errx(1, "Invalid header value");
	}

	memset(names[index], '\0', 128);
	wrapped_read(fd, names[index], lengths[index]);
}


int main(int argc, char* argv[]) {

	if (argc < 2 || argc > 21) {
		errx(1, "Invalid amount of input files");
	}

	for (int i = 0; i < argc - 1; i++) {
		input_fds[i] = wrapped_open(argv[i + 1], O_RDONLY, NULL);
		validate_file(input_fds[i], i);
	}
	
	// първоначален прочит за всеки от файловете
	for (int i = 0; i < argc - 1; i++) {
		wrapped_read(input_fds[i], identifiers + i, sizeof(identifiers[i]));
		wrapped_read(input_fds[i], lengths + i, sizeof(lengths[i]));
	}

	int amount_of_files = argc - 1;

	// въртим цикъл, докато не се затворят всички файлови дескриптори
	while (amount_of_files) {
	
		// basic merge алгоритъм
		uint64_t min_timestamp = UINT64_MAX; 	
		int min_index = -1;

		for (int i = 0; i < argc - 1; i++) {

			// файловите дескриптори, които са неизползваеми са равни на -1
			if (input_fds[i] != -1) {
				
				if (min_timestamp >= identifiers[i]) {
					min_timestamp = identifiers[i];
					min_index = i;
				}

			}
		
		}

		// запазваме репликата с най-малък глобален timestamp
		memset(cue, '\0', 128);
		wrapped_read(input_fds[min_index], cue, lengths[min_index]);
		
		wrapped_write(1, (const char*) names[min_index], strlen(names[min_index]));
		wrapped_write(1, ": ", strlen(": "));
		wrapped_write(1, (const char*) cue, lengths[min_index]);
		wrapped_write(1, "\n", strlen("\n"));

		int w = wrapped_read(input_fds[min_index], identifiers + min_index, sizeof(uint64_t));	
		
		// затваряме файла ако достигнем до края му
		if (w == 0) {
			amount_of_files--;
			close(input_fds[min_index]);
			input_fds[min_index] = -1;
			continue;
		}
		
		wrapped_read(input_fds[min_index], lengths + min_index, sizeof(uint8_t));

		if (w == 0) {
			amount_of_files--;
			close(input_fds[min_index]);
			input_fds[min_index] = -1;
		}
	}
	exit(0);
}

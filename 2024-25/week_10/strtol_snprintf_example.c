#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <err.h>
#include <stdint.h>

int wrapped_write(int fd, const void* buf, int size) {
    
    int w = write(fd, buf, size);

    if (w < 0) {
        err(1, "Failed to write to %d", fd);
    }

    return w;
}

int main(void) {
    

    // за да запишем един int в стринг, така че да може да бъде принтиран в човешки четим формат, трябва да използваме функцията snprintf (man 3 snprintf) 
    // snprintf е единствената позволена функция от <stdio.h>

    // Пример, имаме uint8_t стойността num, която искаме да запишем в някакъв буфер, за да можем да я визуализираме на STDOUT
    uint8_t num = 220;

    // Записваме някакъв байт, който не знаем кой символ реферира в ASCII
    wrapped_write(1, &num, sizeof(num));
    wrapped_write(1, "\n", strlen("\n"));
    
    char buff[10];

    // на snprintf се подава буфера в който ще записваме, неговата дължина, както и начинът, по който искаме да форматираме променливите които ще записваме
    // работим с null-terminated string
    if (snprintf(buff, sizeof(buff), "%d", num) < 0) {
        err(1, "failed to write to buffer");
    }

    wrapped_write(1, buff, strlen(buff));
    wrapped_write(1, "\n", strlen("\n"));
    
    // -----------------------------------------------------------------------------------------

    // Ако искаме да конвертираме в другата посока, тоест имаме стринг който съдържа число, и искаме да извлечем стойността му в някакъв int, използваме функцията strtol (man 3 strtol)
    const char* n = "3";

    // Ако стойността, която се съдържа в стринга overflow-не променливата която сме предназначили да запази резултата, errno се презаписва със стойността ERRANGE
    // strtol взима буфера на числото, както и бройната система към която да го конвертираме
    int result = strtol(n, (char**) NULL, 10);

    if (result == 3) {
        wrapped_write(1, "Success\n", strlen("Success\n"));
    } else {
         wrapped_write(1, "Fail\n", strlen("Fail\n"));
    }
    
   exit(0); 
}

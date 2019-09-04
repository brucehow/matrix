#include "matrix.h"

char *read_line(FILE *fp) {
    size_t size = MEMSIZ;
    char *buffer = allocate(size);
    char ch = fgetc(fp);
    int pos = 0;

    while (ch != EOF && ch != '\n') {
        buffer[pos++] = ch;

        // Dynamically reallocate memory where required
        if (((pos+1)*sizeof(char)) == size) {
            size *= 2;
            buffer = reallocate(buffer, size);
        }
        ch = fgetc(fp);
    }
    buffer[pos] = '\0';
    return buffer;
}

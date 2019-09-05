#include "matrix.h"

char *read_line(FILE *fp) {
    size_t size = MEMSIZ * sizeof(char);
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

enum mat_type read_mat_type(FILE *fp) {
    char* buffer = read_line(fp);
    enum mat_type type;
    if (strcmp(buffer, "int") == 0) {
        type = INT_MAT;
    } else if (strcmp(buffer, "float") == 0) {
        type = FLOAT_MAT;
    } else {
        fprintf(stderr, "Invalid matrix data type '%s'\n", buffer);
        exit(EXIT_FAILURE);
    }
    free(buffer);
    buffer = NULL;
    return type;
}

int read_mat_dim(FILE *fp) {
    char* buffer = read_line(fp);
    int len = strlen(buffer);
    for (int i = 0; i < len; i++) {
        if (!isdigit(buffer[i])) {
            fprintf(stderr, "Invalid matrix row/column value '%s'\n", buffer);
            exit(EXIT_FAILURE);
        }
    }
    int dim = atoi(buffer);
    free(buffer);
    buffer = NULL;
    return dim;
}
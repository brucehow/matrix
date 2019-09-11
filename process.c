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

enum VAR_TYPE read_mat_type(FILE *fp) {
    char* buffer = read_line(fp);
    enum VAR_TYPE type;
    if (strcmp(buffer, "int") == 0) {
        type = TYPE_INT;
    } else if (strcmp(buffer, "float") == 0) {
        type = TYPE_FLOAT;
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

enum VAR_TYPE numeric_type(char *val) {
    int len = strlen(val);
    bool decimal = false; // Ensure we only see 1 decimal if float value
    for (int i = 0; i < len; i++) {
        if (isdigit(val[i])) {
            continue;
        } else if (val[i] == '.' && !decimal) {
            decimal = true;
            continue;
        } else {
            return INVALID;
        }
    }
    return decimal ? TYPE_FLOAT : TYPE_INT;
}
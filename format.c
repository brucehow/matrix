#include "matrix.h"

void check_numeric(char *val) {
    int len = strlen(val);
    for (int i = 0; i < len; i++) {
        if (!isdigit(val[i])) {
            fprintf(stderr, "Invalid value in matrix data '%s'\n", val);
            exit(EXIT_FAILURE);
        }
    }
}

struct COO coo_format(int rows, int cols, enum mat_type type, char *data) {
    // Structure variable initialisation
    struct COO matrix;
    size_t elements_size = MEMSIZ * sizeof(struct ELEMENT);
    matrix.elements = allocate(elements_size);
    matrix.count = 0;
    
    // Data reading variables
    int len, pos = 0;
    size_t size = MEMSIZ * sizeof(char);
    char *val = allocate(size);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            len = 0; // Reset word length to 0

            while (data[pos] != '\0' && data[pos] != ' ') {
                val[len++] = data[pos++];

                // Dynamic memory reallocation for each digit
                if ((len * sizeof(char)) == size) {
                    if (data[pos] != '\0' && data[pos] != ' ') {
                        size += sizeof(char); // Null byte
                    } else {
                        size *= 2;
                    }
                    val = reallocate(val, size);
                }
            }
            val[len] = '\0';
            pos++; // Move away from separating char
            check_numeric(val);
            
            // Zero value filter
            if (type == INT_MAT) {
                int value = atoi(val);
                if (value != 0) {
                    // Dynamically allocate memory for elements struct pointer
                    if (((matrix.count) * sizeof(struct ELEMENT)) == elements_size) {
                        elements_size *= 2;
                        matrix.elements = reallocate(matrix.elements, elements_size);
                    }
                    matrix.elements[matrix.count].value.i = value;
                    matrix.elements[matrix.count].x = i;
                    matrix.elements[matrix.count++].y = j;
                }
            } else {
                float value = atof(val);
                if (value != 0.0) {
                    if ((matrix.count * sizeof(struct ELEMENT)) == elements_size) {
                        elements_size *= 2;
                        matrix.elements = reallocate(matrix.elements, elements_size);
                    }
                    matrix.elements[matrix.count].value.f = value;
                    matrix.elements[matrix.count].x = i;
                    matrix.elements[matrix.count++].y = j;
                }
            }
        }
    }
    free(val);
    val = NULL;

    return matrix;
}

struct CSR csr_format(int rows, int cols, enum mat_type type, char *data) {
    // Structure variable initialisation
    struct CSR matrix;
    size_t nnz_size;
    size_t ja_size = MEMSIZ * sizeof(int);
    if (type == INT_MAT) {
        nnz_size = MEMSIZ * sizeof(int);
        matrix.nnz.i = allocate(nnz_size);
    }  else {
        nnz_size = MEMSIZ * sizeof(float);
        matrix.nnz.f = allocate(nnz_size);
    }
    matrix.ja = allocate(ja_size);
    matrix.ia = allocate(sizeof(int) * (rows+1));
    matrix.ia[0] = 0; // Conventional
    matrix.rows = rows;
    matrix.count = 0;

    // Data reading variables
    int len, pos = 0;
    size_t size = MEMSIZ;
    char *val = allocate(size);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            len = 0; // Reset word length to 0

            while (data[pos] != '\0' && data[pos] != ' ') {
                val[len++] = data[pos++];

                // Dynamic memory reallocation for each digit
                if ((len * sizeof(char)) == size) {
                    if (data[pos] != '\0' && data[pos] != ' ') {
                        size += sizeof(char); // Null byte
                    } else {
                        size *= 2;
                    }
                    val = reallocate(val, size);
                }
            }
            val[len] = '\0';
            pos++; // Move away from separating char
            check_numeric(val);

            // Zero value filter
            if (type == INT_MAT) {
                int value = atoi(val);
                if (value != 0) {
                    // Dynamically allocate memory for nnz and ja pointers
                    if ((matrix.count * sizeof(int)) == ja_size) {
                        nnz_size *= 2;
                        ja_size *= 2;
                        matrix.nnz.i = reallocate(matrix.nnz.i, nnz_size);
                        matrix.ja = reallocate(matrix.ja, ja_size);
                    }
                    matrix.nnz.i[matrix.count] = value;
                    matrix.ja[matrix.count++] = j;
                }
            } else {
                float value = atof(val);
                if (value != 0.0) {
                    if ((matrix.count * sizeof(int)) == ja_size) {
                        nnz_size *= 2;
                        ja_size *= 2;
                        matrix.nnz.f = reallocate(matrix.nnz.f, nnz_size);
                        matrix.ja = reallocate(matrix.ja, ja_size);
                    }
                    matrix.nnz.f[matrix.count] = value;
                    matrix.ja[matrix.count++] = j;
                }
            }
        }
        matrix.ia[i+1] = matrix.count;
    }
    return matrix;
}
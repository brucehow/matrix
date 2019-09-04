#include "matrix.h"

struct COO coo_format(int rows, int cols, char *data) {
    
    // Variable initialisation
    struct COO matrix; 
    matrix.elements = allocate(sizeof(struct ELEMENT));
    matrix.length = 0;
    int len, pos = 0;
    char *val = NULL;
    size_t size;

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            size = MEMSIZ;
            len = 0;
            val = allocate(size);
            
            // Dynamic memory reallocation for each digit
            while (data[pos] != '\0' && data[pos] != ' ') {
                val[len++] = data[pos++];
                if (((len+1)*sizeof(char)) == size) {
                    size *= 2;
                    val = reallocate(val, size);
                }
            }
            val[len] = '\0';
            pos++; // Move away from separating char
            
            // Digit validation and conversion
            for (int i = 0; i < len; i++) {
                if (!isdigit(val[i])) {
                    fprintf(stderr, "Invalid value in matrix data '%s'\n", val);
                    exit(EXIT_FAILURE);
                }
            }
            // Zero value filter
            int value = atoi(val);
            if (value != 0) {
                matrix.elements[matrix.length].value = value;
                matrix.elements[matrix.length].x = i;
                matrix.elements[matrix.length++].y = j;
            }
            val = NULL;
        }
    }
    free(val);
    val = NULL;

    return matrix;
}
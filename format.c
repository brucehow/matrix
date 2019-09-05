#include "matrix.h"

struct COO coo_format(int rows, int cols, enum mat_type type, char *data) {
    
    // Variable initialisation
    struct COO matrix; 
    matrix.elements = allocate(sizeof(struct ELEMENT));
    matrix.length = 0;
    int len, pos = 0;
    size_t size = MEMSIZ;
    char *val = allocate(size);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            len = 0;

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
            if (type == INT) {
                int value = atoi(val);
                if (value != 0) {
                    // Dynamically allocate memory for elements pointer
                    matrix.elements = reallocate(matrix.elements, sizeof(struct ELEMENT) * (matrix.length+1));
                    matrix.elements[matrix.length].value.i = value;
                    matrix.elements[matrix.length].x = i;
                    matrix.elements[matrix.length++].y = j;
                }
            } else {
                float value = atof(val);
                if (value != 0.0) {
                    matrix.elements = reallocate(matrix.elements, sizeof(struct ELEMENT) * (matrix.length+1));
                    matrix.elements[matrix.length].value.f = value;
                    matrix.elements[matrix.length].x = i;
                    matrix.elements[matrix.length++].y = j;
                }
            }
        }
    }
    free(val);
    val = NULL;

    return matrix;
}
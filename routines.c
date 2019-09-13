#include "matrix.h"

void scalar_multiply(struct COO matrix, float scalar) {
    printf("scalar: %f\n", scalar);
    if (matrix.type == TYPE_INT) {
        for (int i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f = (float) matrix.elements[i].value.i * scalar;
        }
    } else {
        for (int i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f *= scalar;
        }
    }
}

void trace(struct COO matrix) {

}

void transpose(struct COO matrix) {

}

void matrix_multiply(struct COO matrix) {
    
}
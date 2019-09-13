#include "matrix.h"

void scalar_multiply(struct COO matrix, float scalar) {
    if (matrix.type == TYPE_INT) {
        #pragma omp parallel for
        for (int i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f = (float) matrix.elements[i].value.i * scalar;
        }
    } else {
        #pragma omp parallel for
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
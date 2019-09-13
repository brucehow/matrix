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

int trace(struct CSR matrix) {
    int trace = 0;

    for (int i = 1; i < matrix.rows + 1; i++) {
        // Check num of elements in row i
        int elements = matrix.ia[i] - matrix.ia[i-1];
        if (elements == 0) {
            continue;
        }
        int pos = matrix.ia[i-1]; // Starting index for ja/nnz
        while (pos - matrix.ia[i-1] <= elements) {
            if (matrix.ja[pos] == i - 1) {
                trace += matrix.nnz.i[pos];
                break;
            } else if (matrix.ja[pos] > i - 1) {
                break;
            }
            pos++;
        }
    }
    return trace;
}

float trace_f(struct CSR matrix) {
    float trace = 0.0;

    for (int i = 1; i < matrix.rows + 1; i++) {
        // Check num of elements in row i
        int elements = matrix.ia[i] - matrix.ia[i-1];
        if (elements == 0) {
            continue;
        }
        int pos = matrix.ia[i-1]; // Starting index for ja/nnz
        while (pos - matrix.ia[i-1] <= elements) {
            if (matrix.ja[pos] == i - 1) {
                trace += matrix.nnz.f[pos];
                break;
            } else if (matrix.ja[pos] > i - 1) {
                break;
            }
            pos++;
        }
    }
    return trace;
}

void transpose(struct COO matrix) {

}

void matrix_multiply(struct COO matrix) {
    
}
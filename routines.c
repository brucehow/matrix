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

    #pragma omp parallel for
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

    #pragma omp parallel for
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

struct CSR matrix_addition(struct CSR matrix, struct CSR matrix2) {
    struct CSR result;
    result.rows = matrix.rows;
    result.cols = matrix.cols;
    result.type = matrix.type;

    size_t nnz_size = matrix.count * sizeof(int);
    result.ia = allocate(sizeof(int) * (result.rows+1));
    result.ja = allocate(nnz_size);
    result.nnz.i = allocate(nnz_size);
    result.count = 0;
    result.ia[0] = 0;

    for (int i = 1; i < matrix.rows + 1; i++) {
        int elements = matrix.ia[i] - matrix.ia[i-1];
        int elements2 = matrix2.ia[i] - matrix2.ia[i-1];
        int pos = matrix.ia[i-1];
        int pos2 = matrix2.ia[i-1];
        int total = elements + elements2;

        while (total > 0) {
            if (pos - matrix.ia[i-1] == elements) { // Matrix no more elements
                while (pos2 - matrix2.ia[i-1] <= elements2) {
                    result.nnz.i[result.count++] = matrix2.nnz.i[pos2++]; 

                    // Dynamic memory allocation
                    if (sizeof(int) * result.count == nnz_size) {
                        nnz_size *= 2;
                        result.nnz.i = reallocate(result.nnz.i, nnz_size);
                        result.ja = reallocate(result.ja, nnz_size);
                    }
                }
                break;
            } else if (pos2 - matrix2.ia[i-1] == elements2) { // Matrix2 no more elements
                while (pos - matrix.ia[i-1] <= elements) {
                    result.nnz.i[result.count++] = matrix.nnz.i[pos++]; 

                    // Dynamic memory allocation
                    if (sizeof(int) * result.count == nnz_size) {
                        nnz_size *= 2;
                        result.nnz.i = reallocate(result.nnz.i, nnz_size);
                        result.ja = reallocate(result.ja, nnz_size);
                    }
                }
                break;
            } else { // Both matrices have elements
                if (matrix.ja[pos] < matrix2.ja[pos2]) {
                    result.ja[result.count] = matrix.ja[pos];
                    result.nnz.i[result.count++] = matrix.nnz.i[pos++];
                } else if (matrix2.ja[pos2] < matrix.ja[pos]) {
                    result.ja[result.count] = matrix2.ja[pos2];
                    result.nnz.i[result.count++] = matrix2.nnz.i[pos2++];
                } else { // Perform addition
                    result.ja[result.count] = matrix.ja[pos];
                    result.nnz.i[result.count++] = matrix.nnz.i[pos++] + matrix2.nnz.i[pos2++];
                    total--;
                }
                // Dynamic memory allocation
                if (sizeof(int) * result.count == nnz_size) {
                    nnz_size *= 2;
                    result.nnz.i = reallocate(result.nnz.i, nnz_size);
                    result.ja = reallocate(result.ja, nnz_size);
                }
                total--;
            }
        }
        result.ia[i] = result.count;
    }
    return result;
}


void transpose(struct COO matrix) {

}

void matrix_multiply(struct COO matrix) {
    
}
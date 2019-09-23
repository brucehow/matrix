#include "matrix.h"

void scalar_multiply(struct COO matrix, double scalar) {
    int i;
    if (matrix.type == TYPE_INT) {
        #pragma omp parallel for shared(matrix,scalar) num_threads(param.threads)
        for (i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f = (double) matrix.elements[i].value.i * scalar;
        }
    } else {
        #pragma omp parallel for shared(matrix,scalar) num_threads(param.threads)
        for (i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f *= scalar;
        }
    }
}

int trace(struct CSR matrix) {
    int trace = 0;
    int i;

    #pragma omp parallel for reduction(+:trace) shared(matrix) num_threads(param.threads)
    for (i = 1; i < matrix.rows + 1; i++) {
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

double trace_f(struct CSR matrix) {
    double trace = 0.0;
    int i;

    #pragma omp parallel for reduction(+:trace) shared(matrix) num_threads(param.threads)
    for (i = 1; i < matrix.rows + 1; i++) {
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

    result.ia = allocate(sizeof(int) * (result.rows+1));
    result.ja = allocate((matrix.count + matrix2.count) * sizeof(int));
    result.nnz.i = allocate((matrix.count + matrix2.count) * sizeof(int));
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
                }
                break;
            } else if (pos2 - matrix2.ia[i-1] == elements2) { // Matrix2 no more elements
                while (pos - matrix.ia[i-1] <= elements) {
                    result.nnz.i[result.count++] = matrix.nnz.i[pos++]; 
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
                total--;
            }
        }
        result.ia[i] = result.count;
    }
    return result;
}

struct CSR matrix_addition_f(struct CSR matrix, struct CSR matrix2) {
    struct CSR result;
    result.rows = matrix.rows;
    result.cols = matrix.cols;
    result.type = matrix.type;

    result.ia = allocate(sizeof(int) * (result.rows+1));
    result.ja = allocate((matrix.count + matrix2.count) * sizeof(int));
    result.nnz.f = allocate((matrix.count + matrix2.count) * sizeof(double));
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
                    result.nnz.f[result.count++] = matrix2.nnz.f[pos2++];
                }
                break;
            } else if (pos2 - matrix2.ia[i-1] == elements2) { // Matrix2 no more elements
                while (pos - matrix.ia[i-1] <= elements) {
                    result.nnz.f[result.count++] = matrix.nnz.f[pos++];
                }
                break;
            } else { // Both matrices have elements
                if (matrix.ja[pos] < matrix2.ja[pos2]) {
                    result.ja[result.count] = matrix.ja[pos];
                    result.nnz.f[result.count++] = matrix.nnz.f[pos++];
                } else if (matrix2.ja[pos2] < matrix.ja[pos]) {
                    result.ja[result.count] = matrix2.ja[pos2];
                    result.nnz.f[result.count++] = matrix2.nnz.f[pos2++];
                } else { // Perform addition
                    result.ja[result.count] = matrix.ja[pos];
                    result.nnz.f[result.count++] = matrix.nnz.f[pos++] + matrix2.nnz.f[pos2++];
                    total--;
                }
                total--;
            }
        }
        result.ia[i] = result.count;
    }
    return result;
}


struct CSR transpose(struct CSC matrix) {
    struct CSR result;
    int i;
    result.rows = matrix.cols;
    result.cols = matrix.rows;
    result.ia = matrix.ia;
    result.ja = matrix.ja;
    result.type = matrix.type;

    if (matrix.type == TYPE_INT) {
        result.nnz.i = allocate(matrix.count * sizeof(int));
        //#pragma omp parallel for shared(result,matrix)
        for (i = 0; i < matrix.count; i++) {
            result.nnz.i[i] = matrix.nnz.i[i];
        }
    } else {
        result.nnz.f = allocate(matrix.count * sizeof(double));
        //#pragma omp parallel for shared(result,matrix)
        for (i = 0; i < matrix.count; i++) {
            result.nnz.f[i] = matrix.nnz.f[i];
        }
    }
    return result;
}

struct COO matrix_multiply(struct CSR matrix, struct CSC matrix2) {
    struct COO result;
    result.count = 0;
    result.type = matrix.type;
    size_t elements_size = MEMSIZ * sizeof(struct ELEMENT);
    result.elements = allocate(elements_size);
    result.rows = matrix.rows;
    result.cols = matrix2.cols;

    for (int i = 0; i < result.rows; i++) {
        int m1count = matrix.ia[i+1] - matrix.ia[i];

        for (int j = 0; j < result.cols; j++) {
            int dp = 0; // Final dot product

            int m1pos = matrix.ia[i];
            int m1seen = 0;
            int m2count = matrix2.ia[j+1] - matrix2.ia[j];
            int m2pos = matrix2.ia[j];
            int m2seen = 0;

            while (m1seen != m1count && m2seen != m2count) {
                if (matrix.ja[m1pos] == matrix2.ja[m2pos]) { // Row and col index match
                    dp += (matrix.nnz.i[m1pos++] * matrix2.nnz.i[m2pos++]);
                    m1seen++;
                    m2seen++;
                } else if (matrix.ja[m1pos] < matrix2.ja[m2pos]) {
                    m1seen++;
                    m1pos++;
                } else {
                    m2seen++;
                    m2pos++;
                }
            }
            if (dp != 0) {
                // Dynamically allocate memory for elements struct pointer
                if (((result.count) * sizeof(struct ELEMENT)) == elements_size) {
                    elements_size *= 2;
                    result.elements = reallocate(result.elements, elements_size);
                }
                result.elements[result.count].value.i = dp;
                result.elements[result.count].x = i;
                result.elements[result.count++].y = j;
            }
        }
    }
    return result;
}

struct COO matrix_multiply_f(struct CSR matrix, struct CSC matrix2) {
    struct COO result;
    result.count = 0;
    result.type = matrix.type;
    result.elements = allocate((matrix.count + matrix2.count) * sizeof(struct ELEMENT));
    result.rows = matrix.rows;
    result.cols = matrix2.cols;

    for (int i = 0; i < result.rows; i++) {
        int m1count = matrix.ia[i+1] - matrix.ia[i];

        for (int j = 0; j < result.cols; j++) {
            double dp = 0; // Final dot product

            int m1pos = matrix.ia[i];
            int m1seen = 0;
            int m2count = matrix2.ia[j+1] - matrix2.ia[j];
            int m2pos = matrix2.ia[j];
            int m2seen = 0;

            while (m1seen != m1count && m2seen != m2count) {
                if (matrix.ja[m1pos] == matrix2.ja[m2pos]) { // Row and col index match
                    dp += (matrix.nnz.f[m1pos++] * matrix2.nnz.f[m2pos++]);
                    m1seen++;
                    m2seen++;
                } else if (matrix.ja[m1pos] < matrix2.ja[m2pos]) {
                    m1seen++;
                    m1pos++;
                } else {
                    m2seen++;
                    m2pos++;
                }
            }
            if (dp != 0) {
                result.elements[result.count].value.f = dp;
                result.elements[result.count].x = i;
                result.elements[result.count++].y = j;
            }
        }
    }
    return result;
}

/**
struct COO matrix_multiply(struct CSR matrix, struct CSC matrix2) {
    struct COO result;
    int i, j;
    result.count = 0;
    result.type = matrix.type;
    result.elements = allocate((matrix.count + matrix2.count) * sizeof(struct ELEMENT));
    result.rows = matrix.rows;
    result.cols = matrix2.cols;

    struct COO *result_local = allocate(result.rows * sizeof(struct COO)); // Remove loop carried dependencies

    for (i = 0; i < result.rows; i++) {
        result_local[i].elements = allocate((matrix.ia[i+1] - matrix.ia[i]) * sizeof(struct ELEMENT));
        result_local[i].count = 0;
    }

    //#pragma omp parallel for shared(matrix,matrix2,result_local)
    for (i = 0; i < result.rows; i++) {
        int m1count = matrix.ia[i+1] - matrix.ia[i];

        for (j = 0; j < result.cols; j++) {
            int dp = 0; // Final dot product

            int m1pos = matrix.ia[i];
            int m1seen = 0;
            int m2count = matrix2.ia[j+1] - matrix2.ia[j];
            int m2pos = matrix2.ia[j];
            int m2seen = 0;

            while (m1seen != m1count && m2seen != m2count) {
                if (matrix.ja[m1pos] == matrix2.ja[m2pos]) { // Row and col index match
                    dp += (matrix.nnz.i[m1pos++] * matrix2.nnz.i[m2pos++]);
                    m1seen++;
                    m2seen++;
                } else if (matrix.ja[m1pos] < matrix2.ja[m2pos]) {
                    m1seen++;
                    m1pos++;
                } else {
                    m2seen++;
                    m2pos++;
                }
            }
            if (dp != 0) {
                // Store values in local result copy
                result_local[i].elements[result_local[i].count].value.i = dp;
                result_local[i].elements[result_local[i].count].x = i;
                result_local[i].elements[result_local[i].count++].y = j;
            }
        }
    }

    // Put local results together
    for (i = 0; i < result.rows; i++) {
        for (j = 0; j < result_local[i].count; j++) {
            result.elements[result.count].value.i = result_local[i].elements[j].value.i;
            result.elements[result.count].x = result_local[i].elements[j].x;
            result.elements[result.count++].y = result_local[i].elements[j].y;
        }
    }
    free(result_local);
    result_local = NULL;
    return result;
}**/
#include "matrix.h"

void scalar_multiply(struct COO matrix, int scalar) {
    if (matrix.type == TYPE_INT) {
        for (int i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.i *= scalar;
        }
    } else {
        for (int i = 0; i < matrix.count; i++) {
            matrix.elements[i].value.f *= scalar;
        }
    }
}
#include "matrix.h"

struct COO coo_format(int rows, int cols, char *data) {

    // Variable initialisation
    struct COO matrix; 
    matrix.elements = allocate(sizeof(struct ELEMENT));

    for (int i = rows; i < rows; i++) {
        for (int j = cols; j < cols; j++) {
            
        }
    }
    
    matrix.elements[0].x = 1;
    matrix.elements[0].y = 2; 
    matrix.elements[0].val = 3;

    return matrix;
}
#include "matrix.h"

int main(int argc, char *argv[]) {
    enum mat_type type;
    int rows, cols;
    char *data = NULL;

    // File validation
    char *filename = "int1.in";
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "%s: No such file\n", filename);
		exit(EXIT_FAILURE);
	}
    
    // Read input file
    type = read_mat_type(fp);
    rows = read_mat_dim(fp);
    cols = read_mat_dim(fp);
    data = read_line(fp);

    printf("%d, %d, %d, %s\n", rows, cols, type, data);

    // Matrix processing
    struct COO matrix = coo_format(rows, cols, type, data);

    /////////////// DEBUG //////////////////
    int limit = matrix.length;
    for (int i = 0; i < limit; i++) {
        printf("(%d,%d,%d) ", matrix.elements[i].x, matrix.elements[i].y, matrix.elements[i].value);
    }
    printf("\n");
    /////////////// DEBUG //////////////////

    free(data);
    exit(EXIT_SUCCESS);
}


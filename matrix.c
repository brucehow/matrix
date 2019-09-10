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

    // Matrix processing
    struct CSC matrix = csc_format(rows, cols, type, data);

    /////////////// DEBUG //////////////////
    int limit = matrix.count;
    printf("Count=%d\nNNZ=", limit);
    for (int i = 0; i < limit; i++) {
        printf("%d,", matrix.nnz.i[i]);
    }
    printf("\nia=");
    for (int i = 0; i < cols+1; i++) {
        printf("%d,", matrix.ia[i]);
    }
    printf("\nja=");
    for (int i = 0; i < limit; i++) {
        printf("%d,", matrix.ja[i]);
    }
    printf("\n");
    /////////////// DEBUG //////////////////

    free(data);
    exit(EXIT_SUCCESS);
}


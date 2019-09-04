#include "matrix.h"

/**
check operation -> call operation -> gets best representation
representaion received in operation func -> do operation
operation funct -> res
*/

int main(int argc, char *argv[]) {
    enum mat_type type;
    int rows, cols;
    char *data;

    // File validation
    char *filename = "int1.in";
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        fprintf(stderr, "%s: No such file\n", filename);
		exit(EXIT_FAILURE);
	}
    
    // Matrix type set
    char *buffer = read_line(fp);
    if (strcmp(buffer, "int") == 0) {
        type = INT;
    } else if (strcmp(buffer, "float") == 0) {
        type = FLOAT;
    } else {
        fprintf(stderr, "Invalid matrix data type '%s'\n", buffer);
        exit(EXIT_FAILURE);
    }
    free(buffer);
    
    // Matrix rows and cols input validation
    buffer = read_line(fp);
    int len = strlen(buffer);
    for (int i = 0; i < len; i++) {
        if (!isdigit(buffer[i])) {
            fprintf(stderr, "Invalid matrix row value '%s'\n", buffer);
            exit(EXIT_FAILURE);
        }
    }
    rows = atoi(buffer);
    free(buffer);

    buffer = read_line(fp);
    len = strlen(buffer);
    for (int i = 0; i < len; i++) {
        if (!isdigit(buffer[i])) {
            fprintf(stderr, "Invalid matrix column value '%s'\n", buffer);
            exit(EXIT_FAILURE);
        }
    }
    cols = atoi(buffer);
    free(buffer);

    // Data parsing
    data = read_line(fp); // free(data);

    // Matrix processing
    struct COO matrix = coo_format(rows, cols, data);

    exit(EXIT_SUCCESS);
}


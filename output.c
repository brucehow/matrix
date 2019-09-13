#include "matrix.h"

int filename_start(char *filename, int len) {
    for (int i = len; i >= 0; i--) {
        if (filename[i] == '/') {
            return i+1;
        }
    }
    return 0;
}

char *get_output_name(struct tm tm, char *routine) {
    char *output = allocate(OUTPUT_NAME_SIZE * sizeof(char));
    snprintf(output, OUTPUT_NAME_SIZE, "%d_%02d%02d%04d_%02d%02d_%s.out", SID, tm.tm_mday, tm.tm_mon + 1, tm.tm_year + 1900, tm.tm_hour, tm.tm_min, routine);
    output[OUTPUT_NAME_SIZE] = '\0';
    return output;
}

void write_details(FILE *fp, char* filename, char* filename2, int rows, int cols, enum ROUTINE_TYPE routine, enum VAR_TYPE type) {
    switch (routine) {
        case SM:
            fprintf(fp, "sm\n");
            break;
        case TR:
            fprintf(fp, "tr\n");
            break;
        case AD:
            fprintf(fp, "ad\n");
            break;
        case TS:
            fprintf(fp, "ts\n");
            break;
        case MM:
            fprintf(fp, "mm\n");
            break;
        default: // Should not get here
            fprintf(stderr, "matrix: could not write routine type to output file\n");
            exit(EXIT_FAILURE);
    }
    int len = strlen(filename);
    int start = filename_start(filename, len);
    fprintf(fp, "%.*s\n", len - start, filename + start);
    if (filename2 != NULL) {
        fprintf(fp, "%s\n", filename2);
    }
    fprintf(fp, "%d\n", omp_get_thread_num());
    switch (type) {
        case TYPE_INT:
            fprintf(fp, "int\n");
            break;
        case TYPE_FLOAT:
            fprintf(fp, "float\n");
            break;
        default: // Should not get here
            fprintf(stderr, "matrix: could not write matrix data type to output file\n");
            exit(EXIT_FAILURE);
    }
    fprintf(fp, "%d\n%d\n", rows, cols);
}

void write_coo_data(FILE *fp, struct COO matrix) {
    int pos = 0;
    int count = 0;
    int total = matrix.rows * matrix.cols;

    if (matrix.type == TYPE_INT) {
        for (int i = 0; i < matrix.rows; i++) {
            for (int j = 0; j < matrix.cols; j++) {
                if (matrix.elements[pos].x != i) { // Skip checking elements if none are on the same row
                    fprintf(fp, "0");
                    count += matrix.cols - j;
                    for (int k = j; k < matrix.cols-1; k++) {
                        fprintf(fp, " 0");
                    }
                    if (count != total) { // Write space if not last val
                        fprintf(fp, " ");
                    }
                    break; // Skip to next row
                } else if (matrix.elements[pos].y == j) {
                    fprintf(fp, "%d", matrix.elements[pos++].value.i);
                } else {
                    fprintf(fp, "0");
                }
                count++;
                if (count != total) { // Write space if not last val
                    fprintf(fp, " ");
                }
            }
        }
    } else {
        for (int i = 0; i < matrix.rows; i++) {
            for (int j = 0; j < matrix.cols; j++) {
                if (matrix.elements[pos].x != i) { // Skip checking elements if none are on the same row
                    fprintf(fp, "0.0");
                    count += matrix.cols - j;
                    for (int k = j; k < matrix.cols-1; k++) {
                        fprintf(fp, " 0.0");
                    }
                    if (count != total) { // Write space if not last val
                        fprintf(fp, " ");
                    }
                    break; // Skip to next row
                } else if (matrix.elements[pos].y == j) {
                    fprintf(fp, "%f", matrix.elements[pos++].value.f);
                } else {
                    fprintf(fp, "0.0");
                }
                count++;
                if (count != total) { // Write space if not last val
                    fprintf(fp, " ");
                }
            }
        }
    }
    fprintf(fp, "\n");
}

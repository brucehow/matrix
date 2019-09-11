#include "matrix.h"

void usage(char *err) {
    char *usage = "\nusage: matrix {routines} [options] -f matrix1 [matrix2]\n"
                "routines:\n"
                "  --sm alpha\tperform scalar multiplcation with value alpha\n"
                "  --tr\t\tcompute the matrix trace value\n"
                "  --ad\t\tperform matrix addition on two matrices, matrix2 must be specified\n"
                "  --ts\t\ttranspose the given matrix\n"
                "  --mm\t\tperform matrix multiplication on two matrices, matrix2 must be specified\n"
                "options:\n"
                "  -t threads\tspecify the number of execution threads to use\n"
                "  -l log\tspecify the log file to output results to\n";
    if (err != NULL) {
        fprintf(stderr, "matrix: %s", err);
    }
    printf("%s", usage);
}

int main(int argc, char *argv[]) {
    // Variable declarations
    enum VAR_TYPE type = INVALID;
    struct ROUTINE routine;
    routine.type = UNDEF;
    int rows, cols;
    char *data = NULL;
    char *filename = NULL;
    char *filename2 = NULL;
    char *logfile = NULL;
    int arg = 1; // Argument pointer

    // Argument processing
    if (argc < 4) {
        usage("invalid number of arguments supplied\n");
        exit(EXIT_FAILURE);
    }
    while (arg < argc) {
        if (strcmp(argv[arg], "--sm") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            } else if ((arg + 1) == argc) { // Expect alpha value
                usage("no alpha value supplied when using --sm\n");
                exit(EXIT_FAILURE);
            } 
            arg++;
            type = numeric_type(argv[arg]); // Check alpha input type
            if (type == INVALID) {
                usage("alpha value must be numeric\n");
                exit(EXIT_FAILURE);
            } else {
                routine.type = SM;
                switch (type) {
                    case TYPE_INT:
                        routine.param.i = strtoimax(argv[arg], NULL, 10);
                        if (errno == EINVAL || errno == ERANGE) {
                            fprintf(stderr, "Failed to convert alpha value '%s' to int\n", argv[arg]);
                            exit(EXIT_FAILURE);
                        }
                        break;
                    case TYPE_FLOAT:
                        routine.param.f = strtof(argv[arg], NULL);
                        if (errno == ERANGE) {
                            fprintf(stderr, "Failed to convert alpha value '%s' to float\n", argv[arg]);
                            exit(EXIT_FAILURE);
                        }
                        break;
                    case INVALID: // Should not get here
                        fprintf(stderr, "Unable to determine alpha value data type '%s'\n", argv[arg]);
                        exit(EXIT_FAILURE);
                }
            }
        } else if (strcmp(argv[arg], "--tr") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = TR;
        } else if (strcmp(argv[arg], "--ad") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = AD;
        } else if (strcmp(argv[arg], "--ts") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = TS;
        } else if (strcmp(argv[arg], "--mm") == 0) {
            if (routine.type != UNDEF) {
                usage("only one routine may be specified\n");
                exit(EXIT_FAILURE);
            }
            routine.type = MM;
        } else if (strcmp(argv[arg], "-t") == 0) {
            if ((arg + 1) == argc) { // Expect thread value
                usage("number of execution threads must be specified when using -t\n");
                exit(EXIT_FAILURE);
            }
            arg++;
            type = numeric_type(argv[arg]); // Check thread input type
            if (type != TYPE_INT) {
                usage("invalid number of execution threads\n");
                exit(EXIT_FAILURE);
            }  else {
                continue; // TODO: CONVERT TO INT AND ASSIGN TO OMP NUM OF THREADS
                // TODO: NEED TO CHECK FOR MULTIPLE -T PARAM
            }
        } else if (strcmp(argv[arg], "-l") == 0) {
            if (logfile != NULL) { // Logfile already specified
                usage("log parameter should only be used once\n");
                exit(EXIT_FAILURE);
            } else if ((arg + 1) == argc) { // Expect log file
                usage("log file must be specified when using -l\n");
                exit(EXIT_FAILURE);
            } else {
                arg++;
                logfile = argv[arg];
            }
        } else if (strcmp(argv[arg], "-f") == 0) {
            if (filename != NULL) { // File already specified
                usage("file parameter should only be used onece\n");
                exit(EXIT_FAILURE);
            } else if ((arg + 1) == argc) { // Expect matrix file
                usage("at least one matrix file must be specified after -f\n");
                exit(EXIT_FAILURE);
            } else if (strcmp(argv[arg+1], "--sm") == 0 || strcmp(argv[arg+1], "--tr") == 0 || strcmp(argv[arg+1], "--ad") == 0
                    || strcmp(argv[arg+1], "--ts") == 0 || strcmp(argv[arg+1], "--mm") == 0 || strcmp(argv[arg+1], "-t") == 0
                    || strcmp(argv[arg+1], "-l") == 0 || strcmp(argv[arg+1], "-f") == 0) {
                usage("at least one matrix file must be specified after -f\n");
                exit(EXIT_FAILURE);
            } else {
                arg++;
                filename = argv[arg];
                // Check for second matrix file if any
                if ((arg + 1) != argc && strcmp(argv[arg+1], "--sm") != 0 && strcmp(argv[arg+1], "--tr") != 0 && strcmp(argv[arg+1], "--ad") != 0
                    && strcmp(argv[arg+1], "--ts") != 0 && strcmp(argv[arg+1], "--mm") != 0 && strcmp(argv[arg+1], "-t") != 0
                    && strcmp(argv[arg+1], "-l") != 0 && strcmp(argv[arg+1], "-f") != 0) {
                    arg++;
                    filename2 = argv[arg];
                }
            }
        }
        arg++;
    }

    // File validation
    if (filename == NULL) {
        usage("no matrix file specified\n");
        exit(EXIT_FAILURE);
    }
    if (filename2 == NULL) {
        printf("no filename2\n");
    }
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

    fclose(fp);
    free(data);
    exit(EXIT_SUCCESS);
}
